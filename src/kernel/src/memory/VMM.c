#include <memory/VMM.h>
#include <memory/PMM.h>
#include <VGA.h>
#include <stdint.h>
#include <liballoc_1_1.h>
#include <Interrupts.h>
#include <memory.h>

#define ADDRESS_MASK            0xffffffffff000
#define AVAILABLE_MASK          0xe00
#define PAGE_SIZE_MASK          0x80
#define ACCESSED_MASK           0x20
#define CACHE_DISABLED_MASK     0x10
#define WRITE_THROUGH_MASK      0x8
#define USER_SUPERVISOR_MASK    0x4
#define READ_WRITE_MASK         0x2
#define PRESENT_MASK            0x1

// Kernel heap starts at 2MB virtual
#define HEAP_START              0x200000

// 128GB virtual address space for now
// TODO: Unlimited memory with swapping
#define MAX_HEAP_SIZE           256 * 128

#define PAGE_SIZE               4096

typedef struct {
    uint64_t entries[512];
} PT;

PT *pt4;

static PT *allocate_page_table(void) {
    void *table = pmm_allocate_page_table();
    kmemset(table, 0, PAGE_SIZE);
    return (PT *) table;
}

static inline void flush_tlb(void *addr) {
    asm volatile("invlpg [%0]"::"r"((unsigned long) addr) :"memory");
}

bool ensure_page_present(void *virtualaddr) {
    unsigned int pt4_index = ((unsigned long) virtualaddr >> 39) & 0x1ff;
    unsigned int pt3_index = ((unsigned long) virtualaddr >> 30) & 0x1ff;
    unsigned int pt2_index = ((unsigned long) virtualaddr >> 21) & 0x1ff;
    unsigned int page_index = ((unsigned long) virtualaddr >> 12) & 0x1ff;

    bool any_changed = false;

    disable_interrupts();

    // Check P4 entry
    if (!(pt4->entries[pt4_index] & PRESENT_MASK)) {
        // Create new P3 table
        PT *pt3 = allocate_page_table();

        // Point P4 entry to that P3 table
        pt4->entries[pt4_index] = (uint64_t) pt3 | 0x3; // read/write, present
        any_changed = true;
    }

    // Check P3 entry
    PT *pt3 = (PT *) (pt4->entries[pt4_index] & ADDRESS_MASK);
    if (!(pt3->entries[pt3_index] & PRESENT_MASK)) {
        // Create new P2 table
        PT *pt2 = allocate_page_table();

        // Point P3 entry to that P2 table
        pt3->entries[pt3_index] = (uint64_t) pt2 | 0x3; // read/write, present
        any_changed = true;
    }

    // Check P2 entry
    PT *pt2 = (PT *) (pt3->entries[pt3_index] & ADDRESS_MASK);
    if (!(pt2->entries[pt2_index] & PRESENT_MASK)) {
        // Create new P1 table
        PT *page_table = allocate_page_table();

        // Point P2 entry to that P1 table
        pt2->entries[pt2_index] = (uint64_t) page_table | 0x3; // read/write, present
        any_changed = true;
    }

    // Check P1 (page table) entry
    PT *page_table = (PT *) (pt2->entries[pt2_index] & ADDRESS_MASK);
    if (!(page_table->entries[page_index] & PRESENT_MASK)) {
        // Allocate new physical page
        void *page = pmm_allocate_page();

        // Map page to address
        page_table->entries[page_index] = (uint64_t) page | 0x3; // read/write, present
        log("[VMM] Mapped virt %p to phys %p\n", virtualaddr, page);

        flush_tlb(page);
        any_changed = true;
    }

    enable_interrupts();
    return any_changed;
}

bool ensure_pages_present(void *vstart, size_t pages) {
    bool any_changed = false;
    for (size_t i = 0; i < pages; i++) {
        any_changed |= ensure_page_present(vstart + i * PAGE_SIZE);
    }
    return any_changed;
}

__attribute((constructor))
static void vmm_init(void) {
    // Get P4 root
    asm ("mov %0, cr3" : "=r"(pt4));
    log("[VMM] PT4 root at: %p\n", (void *) pt4);
}

typedef struct free_block {
    void *start;            // starting address of this block
    size_t size;                // block size in pages
    struct free_block *prev;    // pointer to previous block
    struct free_block *next;    // pointer to next block
} free_block;

free_block blockchain_base = {
        .start = (void *) HEAP_START,
        .size = MAX_HEAP_SIZE,
};

free_block *blockchain = &blockchain_base; // TODO: could result in freeing non-kmalloc-ed struct

void *vmm_allocate_pages(size_t pages) {
    if (!blockchain) {
        log("[VMM] No free space left\n"); // TODO
        return NULL;
    }

    // find first fit block of empty space
    free_block *block = blockchain;
    while (block && block->size < pages) {
        block = block->next;
    }

    if (!block || block->size < pages) {
        log("[VMM] Not enough space to allocate %lu pages\n", pages); // TODO
        return NULL;
    }

    void *span_start = (void *) block->start;
    if (ensure_pages_present(span_start, pages)) {
        // Try again if paging structures changed
        log("[VMM] Recursive call\n");
        return vmm_allocate_pages(pages);
    }

    // shrink the block
    block->start += pages * PAGE_SIZE;
    block->size -= pages;

    log("[VMM] Allocated %lu pages at %p\n", pages, span_start);
    return span_start;
}

void vmm_free_pages(void *start, size_t pages) {
    log("[VMM] Attempting to free %lu pages at %p\n", pages, start);

    // Check if there are no free blocks at all
    if (!blockchain) {
        // Create first free block
        blockchain = (free_block *) kmalloc(sizeof(free_block));
        *blockchain = (free_block) {
                .start = start,
                .size = pages,
        };
        log("[VMM] Freed %lu pages at %p\n", pages, start);
        return;
    }

    // Find free blocks to the left and right
    free_block *left, *right;

    // Check if no free blocks to the left
    if (blockchain->start > start) {
        left = NULL;
        right = blockchain;
    } else {
        // At least one block to the left, find it
        left = blockchain;
        while (left->next && left->next->start < start) {
            left = left->next;
        }

        right = left->next; // can be null
    }

    // Free space adjacent to the left block
    if (left && start == left->start + left->size * PAGE_SIZE) {
        // Extend left block
        left->size += pages;

        // Try to merge left and right blocks
        if (right && left->start + left->size * PAGE_SIZE == right->start) {
            left->size += right->size;
            left->next = right->next;
            if (right->next) {
                right->next->prev = left;
            }
            kfree(right);
        }
    } else if (right && start + pages * PAGE_SIZE == right->start) {
        // Merge with right block
        right->size += pages;
        right->start -= pages * PAGE_SIZE;
    } else {
        // Insert new block
        free_block *new = (free_block *) kmalloc(sizeof(free_block));
        *new = (free_block) {
                .start = start,
                .size = pages,
                .prev = left,
                .next = right,
        };
    }

    log("[VMM] Freed %lu pages at %p\n", pages, start);
}
