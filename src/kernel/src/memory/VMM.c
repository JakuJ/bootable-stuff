#include <memory/VMM.h>
#include <memory/PMM.h>
#include <VGA/VGA.h>
#include <lib/liballoc_1_1.h>
#include <Interrupts.h>
#include <lib/memory.h>

#define NO_EXEC_BIT            (1UL << 63)
#define ADDRESS_MASK           0xfffffff000
#define IGNORED_2              0xf00
#define GLOBAL_PAGE_BIT        0x100
#define PAGE_SIZE_BIT          0x80
#define IGNORED_1              0x40
#define DIRTY_PAGE_BIT         0x40
#define ACCESSED_BIT           0x20
#define CACHE_DISABLED_BIT     0x10
#define WRITE_THROUGH_BIT      0x8
#define USER_ACCESS_BIT        0x4
#define READ_WRITE_BIT         0x2
#define PRESENT_BIT            0x1

#define DIRECTORY_DEFAULTS    (READ_WRITE_BIT | PRESENT_BIT)

// Kernel heap starts at 2MB virtual
#define HEAP_START              0x200000

// 128MB virtual address space for now (-2 MB linearly mapped)
// TODO: Unlimited memory with swapping
#define MAX_HEAP_SIZE           (256 * 126)

typedef struct {
    uint64_t entries[512];
} PT;

PT *pt4;

__attribute__((constructor))
void vmm_init(void) {
    asm ("mov %0, cr3" : "=r"(pt4));
}

static PT *allocate_page_table(void) {
    void *table = pmm_allocate_page_table();
    kmemset(table, 0, PAGE_SIZE);
    return (PT *) table;
}

static inline void flush_tlb(void *addr) {
    asm volatile("invlpg [%0]"::"r"((unsigned long) addr) :"memory");
}

static void vmm_map_memory(uintptr_t physical, uintptr_t virtual, uint64_t flags) {
    unsigned int pt4_index = (virtual >> 39) & 0x1ff;
    unsigned int pt3_index = (virtual >> 30) & 0x1ff;
    unsigned int pt2_index = (virtual >> 21) & 0x1ff;
    unsigned int page_index = (virtual >> 12) & 0x1ff;

    bool interrupts = are_interrupts_enabled();
    disable_interrupts();

    // Check P4 entry
    if (!(pt4->entries[pt4_index] & PRESENT_BIT)) {
        // Create new P3 table
        PT *pt3 = allocate_page_table();

        // Point P4 entry to that P3 table
        pt4->entries[pt4_index] = (uint64_t) pt3 | flags;
    } else {
        pt4->entries[pt4_index] = (pt4->entries[pt4_index] & ~0xfff) | flags;
    }

    // Check P3 entry
    PT *pt3 = (PT *) (pt4->entries[pt4_index] & ADDRESS_MASK);
    if (!(pt3->entries[pt3_index] & PRESENT_BIT)) {
        // Create new P2 table
        PT *pt2 = allocate_page_table();

        // Point P3 entry to that P2 table
        pt3->entries[pt3_index] = (uint64_t) pt2 | flags;
    } else {
        pt3->entries[pt3_index] = (pt3->entries[pt3_index] & ~0xfff) | flags;
    }

    // Check P2 entry
    PT *pt2 = (PT *) (pt3->entries[pt3_index] & ADDRESS_MASK);
    if (!(pt2->entries[pt2_index] & PRESENT_BIT)) {
        // Create new P1 table
        PT *page_table = allocate_page_table();

        // Point P2 entry to that P1 table
        pt2->entries[pt2_index] = (uint64_t) page_table | flags;
    } else {
        pt2->entries[pt2_index] = (pt2->entries[pt2_index] & ~0xfff) | flags;
    }

    // Check P1 (page table) entry
    PT *page_table = (PT *) (pt2->entries[pt2_index] & ADDRESS_MASK);
    if (!(page_table->entries[page_index] & PRESENT_BIT)) {
        // Map page to address
        if (!physical) {
            physical = (uintptr_t) pmm_allocate_page();
        }

        page_table->entries[page_index] = physical | flags | NO_EXEC_BIT;
    } else {
        page_table->entries[page_index] = (page_table->entries[page_index] & ~0xfff) | flags | NO_EXEC_BIT;
    }

    flush_tlb((void *) physical);

    if (interrupts) {
        enable_interrupts();
    }
}

static void ensure_pages_mapped(void *physical, void *vstart, size_t pages, uint64_t flags) {
    void* arg_phys = physical;
    for (size_t i = 0; i < pages; i++) {
        vmm_map_memory(physical, (uintptr_t) vstart + i * PAGE_SIZE, flags);
        if (physical != NULL) {
            physical += PAGE_SIZE;
        }
    }
    if (physical == NULL) {
        log("[VMM] Mapped fresh pages to %p (%lu pages) (flags: %x)\n", vstart, pages, flags);
    } else {
        log("[VMM] Mapped %p to %p (%lu pages) (flags: %x)\n", arg_phys, vstart, pages, flags);
    }
}

typedef struct free_block {
    void *start;                // starting address of this block
    size_t size;                // block size in pages
    struct free_block *prev;    // pointer to previous block
    struct free_block *next;    // pointer to next block
} free_block;

free_block *blockchain = NULL;
bool first_alloc = true, allocating_base = false;

static void print_blockchain(void) {
    free_block *block = blockchain;
    while (block->prev) {
        block = block->prev;
    }

    blockchain = block;
    size_t total = 0;

    log("Blockchain:\n");
    while (block) {
        log("[%p, %p) (%lu pages)\n", block->start, block->start + block->size * PAGE_SIZE, block->size);
        total += block->size;
        block = block->next;
    }
    log("Total: %lu pages\n", total);
}

size_t pages_allocated_for_blockchain;

void *vmm_mmap(void *physical, size_t pages, bool is_kernel) {
    if (!blockchain) {
        if (first_alloc) {
            // We need to allocate the blockchain first
            if (!allocating_base) {
                // Allocate blockchain
                allocating_base = true;
                blockchain = (free_block *) kmalloc(sizeof(free_block));
                *blockchain = (free_block) {
                        .start = (void *) (HEAP_START + pages_allocated_for_blockchain * PAGE_SIZE),
                        .size = MAX_HEAP_SIZE - pages_allocated_for_blockchain,
                };
            } else {
                // We are inside the kmalloc called from the block above
                allocating_base = false;
                first_alloc = false;
                pages_allocated_for_blockchain = pages;

                void *start = (void *) HEAP_START;
                ensure_pages_mapped(NULL, start, pages, DIRECTORY_DEFAULTS);
                return start;
            }
        } else {
            log("[VMM] No free space left\n");
            return NULL;
        }
    }

    // find first block of empty space that's big enough
    free_block *block = blockchain;
    while (block && block->size < pages) {
        block = block->next;
    }

    if (!block || block->size < pages) {
        log("[VMM] Not enough space to allocate %lu pages\n", pages);
        return NULL;
    }

    void *span_start = (void *) block->start;
    uint64_t flags = DIRECTORY_DEFAULTS | (is_kernel ? 0 : USER_ACCESS_BIT);
    ensure_pages_mapped(physical, span_start, pages, flags);

    // shrink the block
    block->start += pages * PAGE_SIZE;
    block->size -= pages;

    // if block is empty, delete it
    if (!block->size) {
        if (block->prev) {
            block->prev->next = block->next;
        }
        if (block->next) {
            block->next->prev = block->prev;
        }

        // that could be the blockchain base
        if (block == blockchain) {
            blockchain = block->next;
        }

        kfree(block);
    }

    log("[VMM] Allocated %lu pages at %p\n", pages, span_start);
    return span_start;
}

void vmm_free_pages(void *start, size_t pages) {
    // Check if there are no free blocks at all
    if (!blockchain) {
        // Create first free block
        blockchain = (free_block *) kmalloc(sizeof(free_block));
        *blockchain = (free_block) {
                .start = start,
                .size = pages,
        };
        log("[VMM] Freed %lu pages at %p\n", pages, start);
        print_blockchain();
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

        right = left->next;
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
        if (left) {
            left->next = new;
        }
        if (right) {
            right->prev = new;
        }
    }

    log("[VMM] Freed %lu pages at %p\n", pages, start);
    print_blockchain();
}

static void set_page_permission(void *addr, bool is_os, bool is_writable, bool is_executable) {
    uintptr_t virtual = (uintptr_t) addr;
    unsigned int pt4_index = (virtual >> 39) & 0x1ff;
    unsigned int pt3_index = (virtual >> 30) & 0x1ff;
    unsigned int pt2_index = (virtual >> 21) & 0x1ff;
    unsigned int page_index = (virtual >> 12) & 0x1ff;

    bool interrupts = are_interrupts_enabled();
    disable_interrupts();

#define SET_FLAGS(X) X = ((X) & ADDRESS_MASK) | DIRECTORY_DEFAULTS | (is_os ? USER_ACCESS_BIT : 0);

    // Update P4 entry
    SET_FLAGS(pt4->entries[pt4_index])

    // Update P3 entry
    PT *pt3 = (PT *) (pt4->entries[pt4_index] & ADDRESS_MASK);
    SET_FLAGS(pt3->entries[pt3_index])

    // Update P2 entry
    PT *pt2 = (PT *) (pt3->entries[pt3_index] & ADDRESS_MASK);
    SET_FLAGS(pt2->entries[pt2_index])

    // Update P1 (page table) entry
    PT *page_table = (PT *) (pt2->entries[pt2_index] & ADDRESS_MASK);

    SET_FLAGS(page_table->entries[page_index])

#undef SET_FLAGS

    if (!is_executable) {
        page_table->entries[page_index] |= NO_EXEC_BIT;
    }
    if (!is_writable) {
        page_table->entries[page_index] &= ~READ_WRITE_BIT;
    }

    flush_tlb((void *) page_table->entries[page_index]);

    if (interrupts) {
        enable_interrupts();
    }
}

#define SET_PERMISSIONS(X, is_os, is_writable, is_executable) \
    extern int _##X##_START_[], _##X##_END_[]; \
    void *X##_start = (void *) _##X##_START_; \
    while (X##_start < (void *) _##X##_END_) {set_page_permission(X##_start, is_os, is_writable, is_executable); X##_start += PAGE_SIZE;}

void vmm_set_page_permissions(void) {
    SET_PERMISSIONS(BOOT, false, true, true)

    // Special case for whatever resides below the boot sector
    void *lowspace = NULL;
    while (lowspace < BOOT_start) {
        set_page_permission(lowspace, false, true, true);
        lowspace += PAGE_SIZE;
    }

    SET_PERMISSIONS(KERNEL_TEXT, false, false, true)
    SET_PERMISSIONS(KERNEL_RODATA, false, false, false)
    SET_PERMISSIONS(KERNEL_DATA, false, true, false)
    SET_PERMISSIONS(KERNEL_BSS, false, true, false)

    SET_PERMISSIONS(OS_TEXT, true, false, true)
    SET_PERMISSIONS(OS_RODATA, true, false, false)
    SET_PERMISSIONS(OS_DATA, true, true, false)
    SET_PERMISSIONS(OS_BSS, true, true, false)
}

#undef SET_PERMISSIONS
