#pragma once

#include "utility.hpp"

static void checkTypeSizes() {
    // 8-bit types
    static_assert(sizeof(bool) == 1);
    static_assert(sizeof(char) == 1);
    static_assert(sizeof(unsigned char) == 1);
    static_assert(sizeof(uint8_t) == 1);

    // 16-bit types
    static_assert(sizeof(short) == 2);
    static_assert(sizeof(uint16_t) == 2);

    // 32-bit types
    static_assert(sizeof(int) == 4);
    static_assert(sizeof(unsigned) == 4);
    static_assert(sizeof(long) == 4);
    static_assert(sizeof(unsigned long) == 4);
    static_assert(sizeof(uint32_t) == 4);

    // 64-bit types
    static_assert(sizeof(long long) == 8);
    static_assert(sizeof(unsigned long long) == 8);
    static_assert(sizeof(uint64_t) == 8);

    // Floating point types
    static_assert(sizeof(float) == 4);
    static_assert(sizeof(double) == 8);
    static_assert(sizeof(long double) == 12);
}

void strlenTest() {
    static_assert(strlen("\0") == 0);
    static_assert(strlen("Hello") == 5);
    static_assert(strlen("Hi there\n") == 9);
}

void chackAssertions() {
    checkTypeSizes();
    strlenTest();
}