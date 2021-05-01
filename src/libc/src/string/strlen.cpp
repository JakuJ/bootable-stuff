#include <string.hpp>

size_t strlen(const char *str) {
    const char *s = str;
    while (*s) {
        s++;
    }
    return static_cast<size_t>(s - str);
}