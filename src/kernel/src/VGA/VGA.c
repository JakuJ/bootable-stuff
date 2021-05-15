#include <stdint.h>
#include <string.h>
#include <VGA/VGA.h>
#include <memory/VMM.h>
#include <VGA/Font.h>
#include <PortIO.h>

#define TAB_SIZE 4
#define PIXEL uint32_t
#define TEXT_COLOR 0x00ffffff

typedef struct {
    unsigned rowMax;
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
            .colMax = vbe.width / FONT_WIDTH,
            .rowMax = vbe.height / FONT_HEIGHT,
    };
}

void vga_info(void) {
    log("Display info:\n");
    log("\tDisplay size: %d x %d x %d\n", vbe.width, vbe.height, vbe.bpp);
    log("\tFramebuffer at %x physical\n", vbe.physical_buffer);
    log("\tBytes per pixel: %d, per line: %d\n", vbe.bytes_per_pixel, vbe.bytes_per_line);
    log("\tCursor range: %d x %d\n", vbe.x_cur_max, vbe.y_cur_max);
    log("\tTeletype range: %d x %d\n", vga.colMax, vga.rowMax);
    log("\tFont size: %d x %d\n", FONT_WIDTH, FONT_HEIGHT);
}

static PIXEL lerp(PIXEL x, PIXEL y, float p) {
    return (PIXEL) ((float) x * (1.0 - p) + (float) y * p);
}

static PIXEL background(unsigned x __attribute__((unused)), unsigned y __attribute__((unused))) {
    float p = (float) x / (float) vbe.width;
    return lerp(0xad, 0x3c, p) << 16 | lerp(0x53, 0x10, p) << 8 | lerp(0x89, 0x53, p);
}

void clearScreen(void) {
    for (unsigned x = 0; x < vbe.width; x++) {
        for (unsigned y = 0; y < vbe.height; y++) {
            FRAMEBUFFER[x + y * vbe.width] = background(x, y);
        }
    }

    vga.cursorX = vga.cursorY = 0;
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
            unsigned py = (vga.rowMax - 1) * FONT_HEIGHT + y;
            FRAMEBUFFER[x + py * vbe.width] = background(x, py);
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
    for (unsigned x = 0; x < FONT_WIDTH; x++) {
        for (unsigned y = 0; y < FONT_HEIGHT; y++) {
            int set = FONT[character - 32][FONT_HEIGHT - y - 1] & 1 << (FONT_WIDTH - x - 1);
            unsigned px = vga.cursorX * FONT_WIDTH + x;
            unsigned py = vga.cursorY * FONT_HEIGHT + y;
            FRAMEBUFFER[px + py * vbe.width] = set ? TEXT_COLOR : background(px, py);
        }
    }
    vga.cursorX++;
}

static void putChar(char c) {
    outb(0x3F8, c); // QEMU serial port debug output (COM1)
    switch (c) {
        case '\r':
            vga.cursorX = 0;
            break;
        case '\n':
            vga.cursorX = 0;
            vga.cursorY++;
            break;
        case '\t': {
            int howMany = TAB_SIZE - vga.cursorX % TAB_SIZE;
            for (int i = 0; i < howMany; i++) {
                putChar(' ');
            }
            break;
        }
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
