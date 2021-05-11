#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint64_t ds; // Data segment selector
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t ax, cx, dx, bx, useless_sp, bp, si, di;
    uint64_t int_no;
    uint64_t err_code;
    uint64_t ip, cs, e_flags, sp, ss; // Pushed by the processor automatically.
} ISR_Frame;

bool are_interrupts_enabled();

static inline void disable_interrupts(void) {
    asm volatile ("cli");
}

static inline void enable_interrupts(void) {
    asm volatile ("sti");
}
