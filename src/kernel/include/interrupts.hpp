#pragma once

#include <stdint.h>

struct ISR_Frame {
    uint64_t ds; // Data segment selector
    uint64_t di, si, bp, useless_sp, bx, dx, cx, ax;
    uint8_t int_no, err_code; // Interrupt number and error code (if applicable)
    uint64_t ip, cs, e_flags, sp, ss; // Pushed by the processor automatically.
};

bool are_interrupts_enabled();
