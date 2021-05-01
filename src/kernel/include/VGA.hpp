#pragma once

class VGA {
    static short *const FRAMEBUFFER;

    short color = 0x1f00;

    const unsigned rowMin = 0;
    const unsigned rowMax = TT_ROWS;
    const unsigned colMin = 0;
    const unsigned colMax = TT_COLUMNS;

    unsigned cursorX = 0;
    unsigned cursorY = 0;

    void ensureCursorInRange();

public:
    static constexpr unsigned TT_COLUMNS = 80;
    static constexpr unsigned TT_ROWS = 25;

    VGA();

    VGA(unsigned minColumn, unsigned maxColumn, unsigned minRow, unsigned maxRow, short color = 0x1f00);

    void clearScreen();

    void putChar(char c);

    void printf(const char *fmt, ...);
};
