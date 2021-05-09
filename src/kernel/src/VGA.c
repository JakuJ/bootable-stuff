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
    kernel_vga.cursorX = kernel_vga.colMin;
    kernel_vga.cursorY = kernel_vga.rowMin;
    for (unsigned x = kernel_vga.colMin; x < kernel_vga.colMax; x++) {
        for (unsigned y = kernel_vga.rowMin; y < kernel_vga.rowMax; y++) {
            putChar(&kernel_vga, ' ');
        }
    }
}

void ensureCursorInRange(VGA *vga) {
    if (vga->cursorX >= vga->colMax) {
        // wrap around X
        vga->cursorX = vga->colMin;
        vga->cursorY++;
    }

    // wrap around Y
    if (vga->cursorY >= vga->rowMax) {
        vga->cursorX = vga->colMin;
        vga->cursorY = vga->rowMin;
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
