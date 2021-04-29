#pragma once

#include "utility.hpp"

inline bool are_interrupts_enabled() {
    unsigned long flags;
    asm volatile ("pushf\n\t pop %0" : "=g"(flags));
    return flags & (1 << 9);
}


#ifdef NOTCOMPILING
inline void enable_interrupts() {
    asm ("sti");
}

inline void disable_interrupts() {
    asm ("cli");
}

static void IRQ_set_mask(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;

    if (IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);
}

static void IRQ_clear_mask(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;

    if (IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);
}
#endif