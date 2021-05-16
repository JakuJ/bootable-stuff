#pragma once

#include <stddef.h>
#include <stdint.h>

void kmemset(void *ptr, uint8_t value, size_t size);

extern void kmemcpy(void *dst, const void *src, size_t count);

extern void kmemcpy_128(void *dst, const void *src, size_t count);
