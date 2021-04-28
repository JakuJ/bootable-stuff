#pragma once

static void checkTypeSizes() {
    // 8-bit types
    static_assert(sizeof(bool) == 1);
    static_assert(sizeof(char) == 1);
    static_assert(sizeof(unsigned char) == 1);

    // 16-bit types
    static_assert(sizeof(short) == 2);

    // 32-bit types
    static_assert(sizeof(int) == 4);
    static_assert(sizeof(unsigned) == 4);
    static_assert(sizeof(long) == 4);
    static_assert(sizeof(unsigned long) == 4);

    // 64-bit types
    static_assert(sizeof(long long) == 8);
    static_assert(sizeof(unsigned long long) == 8);

    // Floating point types
    static_assert(sizeof(float) == 4);
    static_assert(sizeof(double) == 8);
    static_assert(sizeof(long double) == 12);
}

void chackAssertions() {
    checkTypeSizes();
}