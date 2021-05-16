#pragma once

static inline uint64_t rdtsc(void) {
    uint32_t low, high;
    asm volatile("rdtsc":"=a"(low), "=d"(high));
    return ((uint64_t) high << 32) | low;
}
