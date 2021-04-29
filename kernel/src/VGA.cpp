#include "../include/VGA.hpp"

short *const VGA::FRAMEBUFFER = reinterpret_cast<short *>(0xb8000);

VGA::VGA() {}

VGA::VGA(unsigned int minColumn, unsigned int maxColumn, unsigned int minRow, unsigned int maxRow)
        : rowMin(minRow), rowMax(maxRow), colMin(minColumn), colMax(maxColumn) {
    cursorX = minColumn;
    cursorY = minRow;
}

void VGA::clearScreen() {
    for (auto x = colMin; x < colMax; x++) {
        for (auto y = rowMin; y < rowMax; y++) {
            print(' ');
        }
    }
    cursorX = colMin;
    cursorY = rowMin;
}

void VGA::ensureCursorInRange() {
    if (cursorX >= colMax) {
        // wrap around X
        cursorX = colMin;
        cursorY++;
    }

    // wrap around Y
    if (cursorY >= rowMax) {
        cursorX = colMin;
        cursorY = rowMin;
    }
}

template<>
void VGA::print_impl(const char c, ...) {
    switch (c) {
        case '\r':
            cursorX = colMin;
            break;
        case '\n':
            cursorX = colMin;
            cursorY++;
            break;
        default:
            if (c >= 32 && c <= 126) // printable range of ASCII characters
            {
                FRAMEBUFFER[(cursorX++) + cursorY * 80] = color | c;
            }
            break;
    }
    ensureCursorInRange();
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
