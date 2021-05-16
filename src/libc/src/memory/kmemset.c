#include <memory.h>

void kmemset(void *ptr, uint8_t value, size_t size) {
    uint8_t *pointer = ptr;
    for (size_t i = 0; i < size; i++) {
        pointer[i] = value;
    }
}