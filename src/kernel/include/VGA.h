#pragma once

#define TT_ROWS 25
#define TT_COLUMNS 80

#define WHITE_ON_BLUE 0x1f00
#define WHITE_ON_BLACK 0x0f00

typedef struct {
    unsigned rowMin;
    unsigned rowMax;
    unsigned colMin;
    unsigned colMax;
    unsigned cursorX;
    unsigned cursorY;
    short color;
} VGA;

VGA VGA_init(VGA) __attribute__((const));

void clearScreen(void);

void putChar(VGA *, char) __attribute__((nonnull));

void printf(VGA *, const char *, ...) __attribute__((format (printf, 2, 3), nonnull(1, 2)));

void log(const char*, ...) __attribute__((format (printf, 1, 2), nonnull(1)));
