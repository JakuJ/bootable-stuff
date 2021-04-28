#include "../include/vga.hpp"

short *const VGA::FRAMEBUFFER = reinterpret_cast<short *>(0xb8000);

void VGA::clearScreen() {
    cursorX = cursorY = 0;
    for (size_t x = 0; x < TT_COLUMNS; x++) {
        for (size_t y = 0; y < TT_ROWS; y++) {
            print(' ');
        }
    }
}

void VGA::ensureCursorInRange() {
    if (cursorX >= TT_COLUMNS) {
        // wrap around X
        cursorX = 0;
        cursorY++;
    }

    // wrap around Y
    if (cursorY >= TT_ROWS) {
        cursorX = cursorY = 0;
    }
}

template<>
void VGA::print_impl(const char c, ...) {
    switch (c) {
        case '\r':
            cursorX = 0;
            return;
        case '\n':
            cursorX = 0;
            cursorY++;
            return;
    }

    if (c >= 32 && c <= 126) // printable range of ASCII characters
    {
        FRAMEBUFFER[(cursorX++) + cursorY * 80] = color | c;
        ensureCursorInRange();
    }
}

template<>
void VGA::print_impl(const char *str, ...) {
    for (size_t i = 0; i < strlen(str); i++) {
        print(str[i]);
    }
}

template<>
void VGA::print_impl(bool b, ...) {
    if (b) {
        print("true");
    } else {
        print("false");
    }
}
