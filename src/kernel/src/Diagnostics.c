#include <Diagnostics.h>
#include <string.h>

extern int query_cpu(unsigned int page, int reg_no, int bit);

extern void enable_avx(void);

void print_sections(VGA *vga) {

#define PRINT_SECTION(X) \
    extern int _##X##_START_, _##X##_END_; \
    printf(vga, #X ": %lu bytes, %lu - %lu (%p - %p)\n", \
    (unsigned long)&_##X##_END_ - (unsigned long)&_##X##_START_, \
    (unsigned long)(&_##X##_START_), (unsigned long)(&_##X##_END_), (void*)&_##X##_START_, (void*)&_##X##_END_);

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
    printf(vga, "SSE3: %s\n", btoa(query_cpu(1, 2, 0)));
    printf(vga, "SSSE3: %s\n", btoa(query_cpu(1, 2, 9)));
    printf(vga, "SSE4.1: %s\n", btoa(query_cpu(1, 2, 19)));
    printf(vga, "SSE4.2: %s\n", btoa(query_cpu(1, 2, 20)));
    printf(vga, "SSE4a: %s\n", btoa(query_cpu(0x80000001, 2, 6)));

    int xsave = query_cpu(1, 2, 26);
    printf(vga, "XSAVE: %s\n", btoa(xsave));

    if (xsave) {
        int avx = query_cpu(1, 2, 28);
        printf(vga, "AVX: %s\n", btoa(avx));
        printf(vga, "AVX2: %s\n", btoa(query_cpu(7, 1, 5)));
        printf(vga, "AVX512: %s\n", btoa(query_cpu(7, 1, 16)));
        if (avx) {
            enable_avx();
            printf(vga, "AVX enabled\n");
        }
    }
}