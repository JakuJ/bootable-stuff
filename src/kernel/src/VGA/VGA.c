#include <stdint.h>
#include <string.h>
#include <VGA/VGA.h>
#include <memory/VMM.h>
#include <VGA/Font.h>

#define PIXEL uint32_t
#define TEXT_COLOR 0x00ffffff
#define BACKGROUND 0x000000ff

typedef struct {
    unsigned rowMin;
    unsigned rowMax;
    unsigned colMin;
    unsigned colMax;
    unsigned cursorX;
    unsigned cursorY;
} VGA;

typedef struct {
    uint16_t width, height;
    uint8_t bpp;
    uint32_t physical_buffer;
    uint32_t bytes_per_pixel;
    uint16_t bytes_per_line;
    uint16_t x_cur_max, y_cur_max;
} __attribute__((packed)) vbe_screen_info;

extern vbe_screen_info vbe;
PIXEL *FRAMEBUFFER;
VGA vga;

void vga_init(void) {
    size_t fb_size = vbe.height * vbe.bytes_per_line;
    size_t fb_pages = fb_size / PAGE_SIZE;

    uintptr_t page = vbe.physical_buffer & ~0xfff;

    for (size_t i = 0; i <= fb_pages; i++) {
        vmm_map_memory(page + i * PAGE_SIZE, page + i * PAGE_SIZE);
    }

    FRAMEBUFFER = (PIXEL *) (uintptr_t) vbe.physical_buffer;

    vga = (VGA) {
            .colMax = vbe.x_cur_max,
            .rowMax = vbe.y_cur_max,
    };
}

void clearScreen(void) {
    for (unsigned x = 0; x < vbe.width; x++) {
        for (unsigned y = 0; y < vbe.height; y++) {
            FRAMEBUFFER[x + y * vbe.width] = BACKGROUND;
        }
    }

    vga.cursorX = vga.cursorY = 0;

    log("Size: %d x %d x %d, FB at %x, BPP: %d, BPL: %d, Cur: %d x %d\n",
        vbe.width, vbe.height, vbe.bpp, vbe.physical_buffer,
        vbe.bytes_per_pixel, vbe.bytes_per_line,
        vbe.x_cur_max, vbe.y_cur_max);
}

static void scrollUp(void) {
    for (size_t y = 0 * FONT_HEIGHT; y < (vga.rowMax - 1) * FONT_HEIGHT; y++) {
        for (size_t x = 0 * FONT_WIDTH; x < vga.colMax * FONT_WIDTH; x++) {
            FRAMEBUFFER[x + y * vbe.width] = FRAMEBUFFER[x + (y + FONT_HEIGHT) * vbe.width];
        }
    }
    // Clear last line
    for (size_t y = 0; y < FONT_HEIGHT; y++) {
        for (size_t x = 0 * FONT_WIDTH; x < vga.colMax * FONT_WIDTH; x++) {
            FRAMEBUFFER[x + ((vga.rowMax - 1) * FONT_HEIGHT + y) * vbe.width] = BACKGROUND;
        }
    }
}

static void ensureCursorInRange(void) {
    if (vga.cursorX >= vga.colMax) {
        // wrap around X
        vga.cursorX = 0;
        vga.cursorY++;
    }

    // scroll output
    if (vga.cursorY >= vga.rowMax) {
        scrollUp();
        vga.cursorX = 0;
        vga.cursorY = vga.rowMax - 1;
    }
}

static inline void render(char character) {
    for (int x = 0; x < FONT_WIDTH; x++) {
        for (int y = 0; y < FONT_HEIGHT; y++) {
            int set = FONT[character - 32][FONT_HEIGHT - y - 1] & 1 << (FONT_WIDTH - x - 1);
            FRAMEBUFFER[(vga.cursorX * FONT_WIDTH + x) + (vga.cursorY * FONT_HEIGHT + y) * vbe.width] = set
                                                                                                        ? TEXT_COLOR
                                                                                                        : BACKGROUND;
        }
    }
    vga.cursorX++;
}

static void putChar(char c) {
    switch (c) {
        case '\r':
            vga.cursorX = 0;
            break;
        case '\n':
            vga.cursorX = 0;
            vga.cursorY++;
            break;
        default:
            render(c);
            break;
    }
    ensureCursorInRange();
}

void log(const char *fmt, ...) {
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
