#include <VGA.hpp>
#include <string.hpp>

short *const VGA::FRAMEBUFFER = reinterpret_cast<short *>(0xb8000);

VGA::VGA() {}

VGA::VGA(unsigned int minColumn, unsigned int maxColumn, unsigned int minRow, unsigned int maxRow, short color)
        : color(color), rowMin(minRow), rowMax(maxRow), colMin(minColumn), colMax(maxColumn) {
    cursorX = minColumn;
    cursorY = minRow;
}

void VGA::clearScreen() {
    cursorX = colMin;
    cursorY = rowMin;
    for (auto x = colMin; x < colMax; x++) {
        for (auto y = rowMin; y < rowMax; y++) {
            putChar(' ');
        }
    }
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

void VGA::putChar(char c) {
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

void VGA::printf(const char *fmt, ...) {
    va_list arg;
    va_start(arg, fmt);

    char buffer[512];
    vsprintf(buffer, fmt, arg);

    char *ptr = buffer;
    while (*ptr) {
        putChar(*ptr++);
    }

    va_end(arg);
}
