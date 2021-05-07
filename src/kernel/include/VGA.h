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

void clearScreen(VGA *);

void putChar(VGA *, char);

void printf(VGA *, const char *, ...);
