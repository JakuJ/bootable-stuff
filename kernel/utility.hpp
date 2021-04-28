#pragma once

typedef unsigned long size_t;

constexpr auto strlen(const char *str) {
    const char *s = str;
    while (*s) {
        s++;
    }
    return static_cast<size_t>(s - str);
}
