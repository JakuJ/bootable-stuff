#include <memory/VMM.h>
#include <memory/PMM.h>
#include <VGA.h>
#include <stdint.h>

#define address_mask            0xffffffffff000
#define available_mask          0b111000000000
#define page_size_mask          0b000010000000
#define accessed_mask           0b000000100000
#define cache_disabled_mask     0b000000010000
#define write_through_mask      0b000000001000
#define user_supervisor_mask    0b000000000100
#define read_write_mask         0b000000000010
#define present_mask            0b000000000001

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
    if (p4_entry & present_mask) {
        uint64_t p3_entry = ((PT *) (p4_entry & address_mask))->entries[pt3_index];
        if (p3_entry & present_mask) {
            uint64_t p2_entry = ((PT *) (p3_entry & address_mask))->entries[pt2_index];
            if (p2_entry & present_mask) {
                uint64_t p1_entry = ((PT *) (p2_entry & address_mask))->entries[pt1_index];
                if (p1_entry & present_mask) {
                    return (void *) (p1_entry & ~0xfff + page_offset);
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
    log("Virt: %p, phys: %p, values: %lx (virt) %lx (phys)\n", addr, virt2phys(addr), *(uint64_t *) addr, *(uint64_t *) virt2phys(addr));
}

void *vmm_allocate_pages(size_t pages) {
    return NULL;
}

void vmm_free_pages(void *start, size_t pages) {

}
