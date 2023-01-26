#include <Ring3.h>
#include <stdint.h>
#include <stddef.h>
#include <VGA/VGA.h>

typedef struct {
    uint16_t length;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t flags1;
    uint8_t flags2;
    uint8_t base_high;
    uint32_t base_upper;
    uint32_t reserved;
} __attribute__((packed)) tss_entry_t;

typedef struct {
    uint32_t reserved0;
    uint64_t rsp[3];
    uint64_t reserved1;
    uint64_t ist[7];
    uint32_t reserved2;
    uint32_t reserved3;
    uint16_t reserved4;
    uint16_t iopb_offset;
} __attribute__((packed)) tss_t;

extern tss_entry_t GDT64_TSS_ADDR;
tss_t tss;

extern uintptr_t kernel_stack_top;
extern uintptr_t tss_stack_top;

extern void flush_tss(void);

extern void jump_usermode(void);

static void setup_tss(void) {
    tss_entry_t *entry = (tss_entry_t *) (&GDT64_TSS_ADDR);
    *entry = (tss_entry_t) {
            .length = sizeof(tss_entry_t) - 1, // -1 or not -1?
            .base_low = (uintptr_t) & tss & 0xffff,
            .base_mid = ((uintptr_t) & tss >> 16) & 0xff,
            .flags1 = 0x89, // 0b10001001
            .flags2 = 0x20, // 0b00100000
            .base_high = ((uintptr_t) & tss >> 24) & 0xff,
            .base_upper = (uintptr_t) & tss >> 32,
            .reserved = 0,
    };

    uint64_t new_stack = (uint64_t)(&tss_stack_top);

    tss.rsp[0] = (uintptr_t) new_stack;
    tss.iopb_offset = sizeof(tss);

    log("TSS stack at: %lx\n", new_stack);
    flush_tss();
}

void enter_user_mode(void) {
    setup_tss();
    jump_usermode();
}
