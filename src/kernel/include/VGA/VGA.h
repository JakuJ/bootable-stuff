#pragma once

#include <stdint.h>
#include <VGA/Font.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PIXEL uint32_t

#define TT_WIDTH (SCREEN_WIDTH / FONT_WIDTH)
#define TT_HEIGHT (SCREEN_HEIGHT / FONT_HEIGHT)

#define BLUE 0x000000ff
#define WHITE 0x00ffffff

typedef struct {
    unsigned rowMin;
    unsigned rowMax;
    unsigned colMin;
    unsigned colMax;
    unsigned cursorX;
    unsigned cursorY;
    PIXEL color;
} VGA;

void clearScreen(void);

void putChar(VGA *, char) __attribute__((nonnull));

void printf(VGA *, const char *, ...) __attribute__((format (printf, 2, 3), nonnull(1, 2)));

void log(const char*, ...) __attribute__((format (printf, 1, 2), nonnull(1)));
