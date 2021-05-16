#pragma once

#include <stdint.h>
#include <stdbool.h>

bool are_interrupts_enabled();

static inline void disable_interrupts(void) {
    asm volatile ("cli");
}

static inline void enable_interrupts(void) {
    asm volatile ("sti");
}
