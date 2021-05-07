#include <Diagnostics.h>

extern int query_cpu(unsigned int page, int reg_no, int bit);

extern void enable_avx(void);

void print_sections(VGA *vga) {

#define PRINT_SECTION(X) \
    extern int _##X##_START_, _##X##_END_; \
    printf(vga, #X ": %d bytes, %d - %d (0x%x - 0x%x)\n", \
    (long)(&_##X##_END_) - (long)(&_##X##_START_), \
    &_##X##_START_, &_##X##_END_, &_##X##_START_, &_##X##_END_);

    printf(vga, "Sections:\n");
    PRINT_SECTION(BOOT)
    PRINT_SECTION(GDT)
    PRINT_SECTION(TEXT)
    PRINT_SECTION(RODATA)
    PRINT_SECTION(DATA)
    PRINT_SECTION(BSS)
}

void print_sse(VGA *vga) {
    printf(vga, "SSE support:\n");
    printf(vga, "SSE3: %b\n", query_cpu(1, 2, 0));
    printf(vga, "SSSE3: %b\n", query_cpu(1, 2, 9));
    printf(vga, "SSE4.1: %b\n", query_cpu(1, 2, 19));
    printf(vga, "SSE4.2: %b\n", query_cpu(1, 2, 20));
    printf(vga, "SSE4a: %b\n", query_cpu(0x80000001, 2, 6));


    int xsave = query_cpu(1, 2, 26);
    printf(vga, "XSAVE: %b\n", xsave);

    if (xsave) {
        int avx = query_cpu(1, 2, 28);
        printf(vga, "AVX: %b\n", avx);
        printf(vga, "AVX2: %b\n", query_cpu(7, 1, 5));
        printf(vga, "AVX512: %b\n", query_cpu(7, 1, 16));
        if (avx) {
            enable_avx();
            printf(vga, "AVX enabled\n");
        }
    }
}