#include <VGA/VGA.h>
#include <string.h>

#define FRAMEBUFFER ((unsigned char *) 0xa0000)

VGA kernel_vga = {
        .colMax = TT_WIDTH,
        .rowMax = TT_HEIGHT,
        .color = WHITE,
};

void clearScreen(void) {
    for (unsigned x = 0; x < SCREEN_WIDTH; x++) {
        for (unsigned y = 0; y < SCREEN_HEIGHT; y++) {
            FRAMEBUFFER[x + y * SCREEN_WIDTH] = BLUE;
        }
    }
    kernel_vga.cursorX = kernel_vga.colMin;
    kernel_vga.cursorY = kernel_vga.rowMin;
}

static void scrollUp(VGA *vga) {
    for (size_t y = vga->rowMin * FONT_HEIGHT; y < (vga->rowMax - 1) * FONT_HEIGHT; y++) {
        for (size_t x = vga->colMin * FONT_WIDTH; x < vga->colMax * FONT_WIDTH; x++) {
            FRAMEBUFFER[x + y * SCREEN_WIDTH] = FRAMEBUFFER[x + (y + FONT_HEIGHT) * SCREEN_WIDTH];
        }
    }
    // Clear last line
    for (size_t y = 0; y < FONT_HEIGHT; y++) {
        for (size_t x = vga->colMin * FONT_WIDTH; x < vga->colMax * FONT_WIDTH; x++) {
            FRAMEBUFFER[x + ((vga->rowMax - 1) * FONT_HEIGHT + y) * SCREEN_WIDTH] = BLUE;
        }
    }
}

static void ensureCursorInRange(VGA *vga) {
    if (vga->cursorX >= vga->colMax) {
        // wrap around X
        vga->cursorX = vga->colMin;
        vga->cursorY++;
    }

    // scroll output
    if (vga->cursorY >= vga->rowMax) {
        scrollUp(vga);
        vga->cursorX = vga->colMin;
        vga->cursorY = vga->rowMax - 1;
    }
}

static inline void render(VGA *vga, char character) {
    for (int x = 0; x < FONT_WIDTH; x++) {
        for (int y = 0; y < FONT_HEIGHT; y++) {
            int set = FONT[character - 32][FONT_HEIGHT - y - 1] & 1 << (FONT_WIDTH - x - 1);
            uint8_t clr = set ? vga->color : BLUE;
            FRAMEBUFFER[(vga->cursorX * FONT_WIDTH + x) + (vga->cursorY * FONT_HEIGHT + y) * SCREEN_WIDTH] = clr;
        }
    }
    vga->cursorX++;
}

void putChar(VGA *vga, char c) {
    switch (c) {
        case '\r':
            vga->cursorX = vga->colMin;
            break;
        case '\n':
            vga->cursorX = vga->colMin;
            vga->cursorY++;
            break;
        default:
            render(vga, c);
            break;
    }
    ensureCursorInRange(vga);
}

void printf(VGA *vga, const char *fmt, ...) {
    va_list arg;
    va_start(arg, fmt);

    char buffer[512];
    vsprintf(buffer, fmt, arg);

    char *ptr = buffer;
    while (*ptr) {
        putChar(vga, *ptr++);
    }

    va_end(arg);
}

void log(const char *fmt, ...) {
    va_list arg;
    va_start(arg, fmt);

    char buffer[512];
    vsprintf(buffer, fmt, arg);

    char *ptr = buffer;
    while (*ptr) {
        putChar(&kernel_vga, *ptr++);
    }

    va_end(arg);
}
