#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint64_t ds; // Data segment selector
    uint64_t di, si, bp, useless_sp, bx, dx, cx, ax;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
    uint8_t int_no, err_code; // Interrupt number and error code (if applicable)
    uint64_t ip, cs, e_flags, sp, ss; // Pushed by the processor automatically.
} ISR_Frame;

bool are_interrupts_enabled();

static inline void disable_interrupts(void) {
    asm volatile ("cli");
}

static inline void enable_interrupts(void) {
    asm volatile ("sti");
}
