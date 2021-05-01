#pragma once

#include "utility.hpp"

inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port));
}

inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0"
    : "=a"(ret)
    : "Nd"(port));
    return ret;
}

inline void io_wait(void) {
    asm volatile ( "outb %%al, $0x80" : : "a"(0));
}
