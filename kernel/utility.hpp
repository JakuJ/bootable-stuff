#pragma once

typedef unsigned long size_t;

constexpr size_t strlen(const char *str) {
    return *str ? 1 + strlen(str + 1) : 0;
}
