#include <Diagnostics.hpp>

extern "C" int query_cpu(int page, int reg_no, int bit);
extern "C" void enable_avx(void);

void print_sections(VGA &vga) {

#define PRINT_SECTION(X) \
    extern int _##X##_START_, _##X##_END_; \
    vga.printf(#X ": %d bytes, %d - %d (0x%x - 0x%x)\n", \
    (long)(&_##X##_END_) - (long)(&_##X##_START_), \
    &_##X##_START_, &_##X##_END_, &_##X##_START_, &_##X##_END_);

    vga.printf("Sections:\n");
    PRINT_SECTION(BOOT)
    PRINT_SECTION(GDT)
    PRINT_SECTION(TEXT)
    PRINT_SECTION(RODATA)
    PRINT_SECTION(DATA)
    PRINT_SECTION(BSS)
}

void print_sse(VGA &vga) {
    vga.printf("SSE support:\n");
    vga.printf("SSE3: %b\n", query_cpu(1, 2, 0));
    vga.printf("SSSE3: %b\n", query_cpu(1, 2, 9));
    vga.printf("SSE4.1: %b\n", query_cpu(1, 2, 19));
    vga.printf("SSE4.2: %b\n", query_cpu(1, 2, 20));
    vga.printf("SSE4a: %b\n", query_cpu(0x80000001, 2, 6));


    int xsave = query_cpu(1, 2, 26);
    vga.printf("XSAVE: %b\n", xsave);

    if (xsave) {
        int avx = query_cpu(1, 2, 28);
        vga.printf("AVX: %b\n", avx);
        vga.printf("AVX2: %b\n", query_cpu(7, 1, 5));
        vga.printf("AVX512: %b\n", query_cpu(7, 1, 16));
        if (avx) {
            enable_avx();
            vga.printf("AVX enabled\n");
        }
    }
}