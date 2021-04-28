#pragma once

#include "utility.hpp"

class VGA {
    static constexpr unsigned TT_COLUMNS = 80;
    static constexpr unsigned TT_ROWS = 25;
    static short *const FRAMEBUFFER;

    short color = 0x1f00;

    unsigned cursorX = 0;
    unsigned cursorY = 0;

    void ensureCursorInRange();

public:
    void clearScreen();

    void putChar(const char c);

    void putStr(const char *str);
};
