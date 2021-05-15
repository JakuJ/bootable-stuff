#pragma once

void vga_init(void);

void clearScreen(void);

void log(const char *, ...) __attribute__((format (printf, 1, 2), nonnull(1)));
