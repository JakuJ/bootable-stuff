#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint16_t width, height;
    uint8_t bpp;
    uint32_t physical_buffer;
    uint32_t bytes_per_pixel;
    uint16_t bytes_per_line;
    uint16_t x_cur_max, y_cur_max;
} __attribute__((packed)) vbe_screen_info;

void vga_init(void);

void vga_info(void);

void clearScreen(void);

void log(const char *, ...) __attribute__((format (printf, 1, 2), nonnull(1)));

void log_n(char *, size_t);
