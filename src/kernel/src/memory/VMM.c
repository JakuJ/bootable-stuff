#include <memory/VMM.h>
#include <memory/PMM.h>
#include <VGA.h>
#include <stdint.h>
#include <liballoc_1_1.h>

#define ADDRESS_MASK            0xffffffffff000
#define AVAILABLE_MASK          0xe00
#define PAGE_SIZE_MASK          0x80
#define ACCESSED_MASK           0x20
#define CACHE_DISABLED_MASK     0x10
#define WRITE_THROUGH_MASK      0x8
#define USER_SUPERVISOR_MASK    0x4
#define READ_WRITE_MASK         0x2
#define PRESENT_MASK            0x1

// Kernel heap starts at 1MB virtual
#define HEAP_START              0x100000

// 4GB virtual address space for now
// TODO: Unlimited memory with swapping
#define MAX_HEAP_SIZE           256 * 1024 * 4

#define PAGE_SIZE               4096

typedef struct {
    uint64_t entries[512];
} PT;

PT *pt4;

void *virt2phys(void *virtualaddr) {
    unsigned int pt4_index = ((unsigned long) virtualaddr >> 39) & 0x1ff;
    unsigned int pt3_index = ((unsigned long) virtualaddr >> 30) & 0x1ff;
    unsigned int pt2_index = ((unsigned long) virtualaddr >> 21) & 0x1ff;
    unsigned int pt1_index = ((unsigned long) virtualaddr >> 12) & 0x1ff;
    unsigned int page_offset = (unsigned long) virtualaddr & 0xfff;

    log("[VMM] Accessing memory at indices: %d, %d, %d, %d, %d\n", pt4_index, pt3_index, pt2_index, pt1_index,
        page_offset);

    uint64_t p4_entry = pt4->entries[pt4_index];
    if (p4_entry & PRESENT_MASK) {
        uint64_t p3_entry = ((PT *) (p4_entry & ADDRESS_MASK))->entries[pt3_index];
        if (p3_entry & PRESENT_MASK) {
            uint64_t p2_entry = ((PT *) (p3_entry & ADDRESS_MASK))->entries[pt2_index];
            if (p2_entry & PRESENT_MASK) {
                uint64_t p1_entry = ((PT *) (p2_entry & ADDRESS_MASK))->entries[pt1_index];
                if (p1_entry & PRESENT_MASK) {
                    return (void *) ((p1_entry & ~0xfff) + page_offset);
                } else {
                    log("PT1 entry %d not present: %lx\n", pt1_index, p1_entry);
                }
            } else {
                log("PT2 entry %d not present: %lx\n", pt2_index, p2_entry);
            }
        } else {
            log("PT3 entry %d not present: %lx\n", pt3_index, p3_entry);
        }
    } else {
        log("PT4 entry %d not present: %lx\n", pt4_index, p4_entry);
    }
    return NULL;
}

void vmm_init(void) {
    pmm_init();

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

    // shrink the block
    void *span_start = (void *) block->start;
    block->start += pages * PAGE_SIZE;
    block->size -= pages;

    // TODO: Ensure page is present
    virt2phys(span_start);

    log("[VMM] Allocated %lu pages at %p\n", pages, span_start);
    return span_start;
}

void vmm_free_pages(void *start, size_t pages) {
    // Check if there are no free blocks at all
    if (!blockchain) {
        // Create first free block
        blockchain = kmalloc(sizeof(free_block));
        *blockchain = (free_block) {
                .start = start,
                .size = pages,
        };
        log("[VMM] Freeing %lu pages at %p\n", pages, start);
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
        free_block *new = kmalloc(sizeof(free_block));
        *new = (free_block) {
                .start = start,
                .size = pages,
                .prev = left,
                .next = right,
        };
    }

    log("[VMM] Freeing %lu pages at %p\n", pages, start);
}
