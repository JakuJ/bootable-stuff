#include <memory/PMM.h>
#include <stdint.h>
#include <stddef.h>
#include <lib/memory.h>

// TODO: Respect the memory map
#define RESERVED_START  0x100000
#define MEM_START       0x200000
#define PAGE_SIZE       4096

// Page manager sees 128MB of RAM
#define PAGES (128 * 4)

uint64_t bitmap[PAGES];

__attribute__((constructor))
static void pmm_init(void) {
    kmemset(bitmap, 0xff, sizeof(bitmap));
}

static void *allocate_page_from(uintptr_t start) {
    size_t i = (start - RESERVED_START) / PAGE_SIZE / 64;

    for (; i < PAGES; i++) {
        uint64_t first_set;
        int zero;
        asm ("bsr %0, %2" : "=r" (first_set), "=@ccz"(zero) : "mr" (bitmap[i]));

        if (__builtin_expect(zero, 0)) continue;

        if (first_set != 0) {
            bitmap[i] &= ~(1UL << first_set);
            return (void *) (RESERVED_START + PAGE_SIZE * (64 * i + (63 - first_set)));
        } else {
            bitmap[i] = 0;
            return (void *) (RESERVED_START + PAGE_SIZE * (64 * i + 63));
        }
    }
    return NULL; // TODO: Memory exhausted, swap or panic
}

void *pmm_allocate_page_table(void) {
    return allocate_page_from(RESERVED_START);
}

void *pmm_allocate_page(void) {
    return allocate_page_from(MEM_START);
}