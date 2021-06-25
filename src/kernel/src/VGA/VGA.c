#include <stdint.h>
#include <lib/string.h>
#include <VGA/VGA.h>
#include <memory/VMM.h>
#include <VGA/Font.h>
#include <PortIO.h>
#include <lib/liballoc_1_1.h>
#include <lib/memory.h>

#define TAB_SIZE 4
#define PIXEL uint32_t
#define TEXT_COLOR 0x00ffffff

typedef struct {
    unsigned rowMax;
    unsigned colMax;
    unsigned cursorX;
    unsigned cursorY;
} VGA;

extern vbe_screen_info vbe_info;

PIXEL *FRAMEBUFFER, *BACKBUFFER, *BACKGROUND;

VGA vga;
bool vga_ready = false;

size_t fb_size;

void vga_init(void) {
    fb_size = vbe_info.height * vbe_info.bytes_per_line;
    size_t fb_pages = fb_size / PAGE_SIZE;

    uintptr_t page = vbe_info.physical_buffer & ~0xfff;

    for (size_t i = 0; i <= fb_pages; i++) {
        vmm_map_memory(page + i * PAGE_SIZE, page + i * PAGE_SIZE);
    }

    FRAMEBUFFER = (PIXEL *) (uintptr_t) vbe_info.physical_buffer;

    vga = (VGA) {
            .colMax = vbe_info.width / FONT_WIDTH,
            .rowMax = vbe_info.height / FONT_HEIGHT,
    };

    // no need to calloc as we clear them anyway
    BACKBUFFER = (PIXEL *) kmalloc(fb_size);
    BACKGROUND = (PIXEL *) kmalloc(fb_size);

    // Fill background buffer with some texture
    const PIXEL C1 = 0x232526, C2 = 0x414345, C3 = 0xf37335;
    for (size_t y = 0; y < vbe_info.height; y++) {
        int flag1 = 0;
        for (size_t x = 0; x < vbe_info.width; x++) {
            // Checkerboard pattern
            flag1 ^= (x + y) % 100 == 0;

            // Orange color
            int dx = (int) x - vbe_info.width / 2;
            int dy = (int) y - vbe_info.height / 2;
            int flag2 = (dx * dx + dy * dy) <= 10000;

            // Stripes
            float sin;
            asm ("fsin" : "=t"(sin) : "0"((float) (50 * y)));
            flag2 &= sin > 0.0f;

            // Set color
            BACKGROUND[x + y * vbe_info.width] = flag2 ? C3 : (flag1 ? C1 : C2);
        }
    }

    vga_ready = true;
}

static void swapBuffers(void) {
    kmemcpy_128(FRAMEBUFFER, BACKBUFFER, fb_size);
}

void clearScreen(void) {
    if (!vga_ready) return;
    vga.cursorX = vga.cursorY = 0;

    kmemcpy_128(BACKBUFFER, (void *) BACKGROUND, fb_size);
    swapBuffers();
}

static void scrollUp(void) {
    for (size_t y = 0 * FONT_HEIGHT; y < (vga.rowMax - 1) * FONT_HEIGHT; y++) {
        for (size_t x = 0 * FONT_WIDTH; x < (vga.colMax + 1) * FONT_WIDTH; x++) {
            PIXEL lower = BACKBUFFER[x + (y + FONT_HEIGHT) * vbe_info.width];
            BACKBUFFER[x + y * vbe_info.width] = lower == TEXT_COLOR ? TEXT_COLOR : BACKGROUND[x + y * vbe_info.width];
        }
    }
    // Clear last line
    for (size_t y = 0; y < FONT_HEIGHT; y++) {
        for (size_t x = 0 * FONT_WIDTH; x < vga.colMax * FONT_WIDTH; x++) {
            unsigned py = (vga.rowMax - 1) * FONT_HEIGHT + y;
            BACKBUFFER[x + py * vbe_info.width] = BACKGROUND[x + py * vbe_info.width];
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

static void render(char character) {
    for (unsigned x = 0; x < FONT_WIDTH; x++) {
        for (unsigned y = 0; y < FONT_HEIGHT; y++) {
            int set = FONT[character - 32][FONT_HEIGHT - y - 1] & 1 << (FONT_WIDTH - x - 1);
            unsigned px = vga.cursorX * FONT_WIDTH + x;
            unsigned py = vga.cursorY * FONT_HEIGHT + y;
            BACKBUFFER[px + py * vbe_info.width] = set ? TEXT_COLOR : BACKGROUND[px + py * vbe_info.width];
        }
    }
}

static void putChar(char c) {
    // QEMU serial port debug output (COM1)
    outb(0x3F8, c);

    // No graphic output before we initialize everything
    if (__builtin_expect(!vga_ready, 0)) return;

    switch (c) {
        case '\r':
            vga.cursorX = 0;
            break;
        case '\b':
            if (!vga.cursorX) {
                if (vga.cursorY) {
                    vga.cursorY--;
                    vga.cursorX = vga.colMax - 1;
                }
            } else {
                vga.cursorX--;
            }
            render(' ');
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
            vga.cursorX++;
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

    swapBuffers();
    va_end(arg);
}

void log_n(char *buf, size_t n) {
    for (size_t i = 0; i < n; i++) {
        putChar(buf[i]);
    }

    swapBuffers();
}

void vga_info(void) {
    log("Display info:\n");
    log("\tDisplay size: %d x %d x %d\n", vbe_info.width, vbe_info.height, vbe_info.bpp);
    log("\tFramebuffer at %x physical, size: %lukB\n", vbe_info.physical_buffer, fb_size / 1000);
    log("\tBytes per pixel: %d, per line: %d\n", vbe_info.bytes_per_pixel, vbe_info.bytes_per_line);
    log("\tCursor range: %d x %d\n", vbe_info.x_cur_max, vbe_info.y_cur_max);
    log("\tTeletype range: %d x %d\n", vga.colMax, vga.rowMax);
    log("\tFont size: %d x %d\n", FONT_WIDTH, FONT_HEIGHT);
    log("\nPalette test:\n\n");
    for (char c = 1; (size_t) c < 95; c++) {
        putChar(c + 32);
    }
    log("\n");
}
