#pragma once

// These headers are part of the compiler itself, and not the standard library
#include <stddef.h> // size_t and NULL
#include <stdint.h> // intXX_t and uintXX_t types

// Other accessible headers:
// <float.h>, <iso646.h>, <limits.h>, <stdarg.h>

constexpr auto strlen(const char *str) {
    const char *s = str;
    while (*s) {
        s++;
    }
    return static_cast<size_t>(s - str);
}
