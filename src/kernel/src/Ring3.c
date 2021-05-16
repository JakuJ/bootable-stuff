#include <Ring3.h>
#include <stdint.h>
#include <stddef.h>
#include <VGA/VGA.h>

typedef struct {
    uint32_t reserved1;
    uint64_t rsp0, rsp1, rsp2;
    uint64_t reserved2;
    uint64_t ist1, ist2, ist3, ist4, ist5, ist6, ist7;
    uint64_t reserved3;
    uint16_t reserved4;
    uint16_t iomap_base;
} __attribute__((packed)) tss_entry;

extern uintptr_t GDT64_TSS_ADDR;
tss_entry tss;
extern uintptr_t kernel_stack_top;

extern void flush_tss(void);

extern void jump_usermode(void);

static void setup_tss(void) {
    // Compute the base and limit of the TSS for use in the GDT entry.
    uint64_t base = (uint64_t) &tss;
    size_t limit = base + sizeof(tss_entry);

    uint16_t *gdt_16 = (uint16_t *) (uintptr_t) (&GDT64_TSS_ADDR);
    gdt_16[0] = limit & 0xffff; // limit low
    gdt_16[1] = base & 0xffff; // base low

    uint8_t *gdt_8 = (uint8_t *) (uintptr_t) (&GDT64_TSS_ADDR);

    // base middle
    gdt_8[4] = (base >> 16) & 0xff;

    uint8_t gran = gdt_8[6];
    gran &= 0xf0;                           // clear limit high
    gran |= (limit & (0xf << 16)) >> 16;    // set limit high
    gdt_8[6] = gran;

    gdt_8[7] = (base & (0xffUL << 24)) >> 24; // base high

    tss = (tss_entry) {
            .rsp0 = (uintptr_t) (&kernel_stack_top),
    };

    flush_tss();
}

void enter_user_mode(void) {
    setup_tss();
    jump_usermode();
}
