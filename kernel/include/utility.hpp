#pragma once

typedef unsigned long size_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

constexpr auto strlen(const char *str) {
    const char *s = str;
    while (*s) {
        s++;
    }
    return static_cast<size_t>(s - str);
}
