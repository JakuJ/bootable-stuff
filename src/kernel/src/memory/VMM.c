#include <memory/VMM.h>
#include <memory/PMM.h>
#include <VGA.h>
#include <stdint.h>

#define ADDRESS_MASK            0xffffffffff000
#define AVAILABLE_MASK          0b111000000000
#define PAGE_SIZE_MASK          0b000010000000
#define ACCESSED_MASK           0b000000100000
#define CACHE_DISABLED_MASK     0b000000010000
#define WRITE_THROUGH_MASK      0b000000001000
#define USER_SUPERVISOR_MASK    0b000000000100
#define READ_WRITE_MASK         0b000000000010
#define PRESENT_MASK            0b000000000001

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

    log("Indices: %d, %d, %d, %d, %d\n", pt4_index, pt3_index, pt2_index, pt1_index, page_offset);

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
    log("PT4 root at: %p\n", (void *) pt4);

    void *addr = (void *) 0xfffffffffffff000;
    log("Virt: %p, phys: %p, values: %lx (virt) %lx (phys)\n", addr, virt2phys(addr), *(uint64_t *) addr,
        *(uint64_t *) virt2phys(addr));
}

typedef struct free_block {
    uintptr_t start;            // starting address of this block
    size_t size;                // block size in pages
    struct free_block *prev;           // pointer to previous block
    struct free_block *next;           // pointer to next block
} free_block;

free_block blockchain = {
        .start = HEAP_START,
        .size = MAX_HEAP_SIZE,
};

void *vmm_allocate_pages(size_t pages) {
    // find first fit block of empty space
    free_block *block = &blockchain;
    while (block->size < pages && block->next) {
        block = block->next;
    }

    // shrink the block
    void *span_start = (void *) block->start;
    block->start += pages * PAGE_SIZE;
    block->size -= pages;

    // TODO: Ensure page is present
    virt2phys(span_start);

    return span_start;
}

void vmm_free_pages(void *start, size_t pages) {
    // TODO
}
