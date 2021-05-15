#pragma once

#include <stdint.h>
#include <VGA/Font.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

#define TT_WIDTH (SCREEN_WIDTH / FONT_WIDTH)
#define TT_HEIGHT (SCREEN_HEIGHT / FONT_HEIGHT)

#define BLUE 0x1
#define BLACK 0x0
#define WHITE 0xf

typedef struct {
    unsigned rowMin;
    unsigned rowMax;
    unsigned colMin;
    unsigned colMax;
    unsigned cursorX;
    unsigned cursorY;
    uint8_t color;
} VGA;

void clearScreen(void);

void putChar(VGA *, char) __attribute__((nonnull));

void printf(VGA *, const char *, ...) __attribute__((format (printf, 2, 3), nonnull(1, 2)));

void log(const char*, ...) __attribute__((format (printf, 1, 2), nonnull(1)));
