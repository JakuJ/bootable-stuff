#include <memory/PMM.h>
#include <stdint.h>
#include <stddef.h>
#include <memory.h>

// Non-linearly page mapped memory starts at 2MB physical
// TODO: Respect the memory map
#define MEM_START 0x200000
#define PAGE_SIZE 4096

// Bootstrap the page manager with 4MB of RAM
#define PAGES 1024

uint64_t bitmap[PAGES];

void pmm_init(void) {
    memset(bitmap, 0xff, sizeof(bitmap));
}

void *pmm_allocate_page(void) {
    uint64_t first_set;
    for (size_t i = 0; i < sizeof(bitmap); i++) {
        asm ("lzcnt %0, %1" : "=r"(first_set) : "m"(bitmap[i]));
        if (first_set != 0) {
            bitmap[i] &= ~(1UL << first_set);
            return (void *) (MEM_START + PAGE_SIZE * (64 * i + (63 - first_set)));
        } else if (bitmap[i]) {
            bitmap[i] = 0;
            return (void *) (MEM_START + PAGE_SIZE * (64 * i + (63 - first_set)));
        }
    }
    return 0; // TODO: Memory exhausted, swap or panic
}