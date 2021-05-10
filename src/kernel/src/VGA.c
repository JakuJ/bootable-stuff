#include <VGA.h>
#include <string.h>
#include <macros.h>

#define FRAMEBUFFER ((short *) 0xb8000)

VGA VGA_init(VGA vga) {
    DEFAULT(vga.rowMax, TT_ROWS);
    DEFAULT(vga.colMax, TT_COLUMNS);
    DEFAULT(vga.cursorX, vga.colMin);
    DEFAULT(vga.cursorY, vga.rowMin);
    DEFAULT(vga.color, WHITE_ON_BLUE);
    return vga;
}

VGA kernel_vga;

__attribute__((constructor))
static void init_global_VGA(void) {
    kernel_vga = VGA_init((VGA) {0});
}

void clearScreen(void) {
    for (unsigned y = kernel_vga.rowMin; y < kernel_vga.rowMax; y++) {
        for (unsigned x = kernel_vga.colMin; x < kernel_vga.colMax; x++) {
            putChar(&kernel_vga, ' ');
        }
    }
    kernel_vga.cursorX = kernel_vga.colMin;
    kernel_vga.cursorY = kernel_vga.rowMin;
}

static void scrollUp(VGA *vga) {
    for (size_t y = vga->rowMin; y < vga->rowMax - 1; y++) {
        for (size_t x = vga->colMin; x < vga->colMax; x++) {
            FRAMEBUFFER[x + y * 80] = FRAMEBUFFER[x + (y + 1) * 80];
        }
    }
    // Clear last line
    for (size_t x = vga->colMin; x < vga->colMax; x++) {
        FRAMEBUFFER[x + (vga->rowMax - 1) * 80] = vga->color | ' ';
    }
}

static void ensureCursorInRange(VGA *vga) {
    if (vga->cursorX >= vga->colMax) {
        // wrap around X
        vga->cursorX = vga->colMin;
        vga->cursorY++;
    }

    // wrap around Y
    if (vga->cursorY >= vga->rowMax) {
        scrollUp(vga);
        vga->cursorX = vga->colMin;
        vga->cursorY = vga->rowMax - 1;
//        vga->cursorY = vga->rowMin;
    }
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
            if (c >= 32 && c <= 126) // printable range of ASCII characters
            {
                FRAMEBUFFER[(vga->cursorX++) + vga->cursorY * 80] = vga->color | c;
            }
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
