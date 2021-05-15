#include <VGA/VGA.h>
#include <memory/VMM.h>
#include <string.h>

#define PAGE_SIZE 0x1000
#define BACKGROUND BLUE

typedef struct {
    uint16_t width, height;
    uint8_t bpp;
    uint32_t physical_buffer;
    uint32_t bytes_per_pixel;
    uint16_t bytes_per_line;
    uint16_t x_cur_max, y_cur_max;
} __attribute__((packed)) vbe_screen_info;

vbe_screen_info *vbe;
PIXEL *FRAMEBUFFER;

void vga_init(void) {
    extern uintptr_t vbe_screen;
    vbe = (vbe_screen_info *) &vbe_screen;

    size_t fb_size = vbe->height * vbe->bytes_per_line;
    size_t fb_pages = fb_size / PAGE_SIZE;

    uintptr_t page = vbe->physical_buffer;

    for (size_t i = 0; i <= fb_pages; i++) {
        vmm_map_memory(page + i * PAGE_SIZE, page + i * PAGE_SIZE);
    }

    FRAMEBUFFER = (PIXEL *) (uintptr_t) vbe->physical_buffer;
}

VGA kernel_vga = {
        .colMax = TT_WIDTH,
        .rowMax = TT_HEIGHT,
        .color = WHITE,
};

void clearScreen(void) {
    for (unsigned x = 0; x < SCREEN_WIDTH; x++) {
        for (unsigned y = 0; y < SCREEN_HEIGHT; y++) {
            FRAMEBUFFER[x + y * SCREEN_WIDTH] = BACKGROUND;
        }
    }
    kernel_vga.cursorX = kernel_vga.colMin;
    kernel_vga.cursorY = kernel_vga.rowMin;

    log("Size: %d x %d x %d, FB at %x, BPP: %d, BPL: %d, Cur: %d x %d\n",
        vbe->width, vbe->height, vbe->bpp, vbe->physical_buffer,
        vbe->bytes_per_pixel, vbe->bytes_per_line,
        vbe->x_cur_max, vbe->y_cur_max);
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
            FRAMEBUFFER[x + ((vga->rowMax - 1) * FONT_HEIGHT + y) * SCREEN_WIDTH] = BACKGROUND;
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
            PIXEL clr = set ? vga->color : BACKGROUND;
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
