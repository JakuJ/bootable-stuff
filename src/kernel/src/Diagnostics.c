#include <Diagnostics.h>
#include <string.h>
#include <VGA/VGA.h>

extern int query_cpu(unsigned int page, int reg_no, int bit);

extern void enable_avx(void);

void print_sections(void) {

#define PRINT_SECTION(X) \
    extern int _##X##_START_, _##X##_END_; \
    log(#X ": %lu bytes, %lu - %lu (%p - %p)\n", \
    (unsigned long)&_##X##_END_ - (unsigned long)&_##X##_START_, \
    (unsigned long)(&_##X##_START_), (unsigned long)(&_##X##_END_), (void*)&_##X##_START_, (void*)&_##X##_END_);

    log("Sections:\n");
    PRINT_SECTION(BOOT)
    PRINT_SECTION(GDT)
    PRINT_SECTION(TEXT)
    PRINT_SECTION(RODATA)
    PRINT_SECTION(DATA)
    PRINT_SECTION(BSS)
}

void print_sse(void) {
    log("SSE support:\n");
    log("SSE3: %s\n", btoa(query_cpu(1, 2, 0)));
    log("SSSE3: %s\n", btoa(query_cpu(1, 2, 9)));
    log("SSE4.1: %s\n", btoa(query_cpu(1, 2, 19)));
    log("SSE4.2: %s\n", btoa(query_cpu(1, 2, 20)));
    log("SSE4a: %s\n", btoa(query_cpu(0x80000001, 2, 6)));

    int xsave = query_cpu(1, 2, 26);
    log("XSAVE: %s\n", btoa(xsave));

    if (xsave) {
        int avx = query_cpu(1, 2, 28);
        log("AVX: %s\n", btoa(avx));
        log("AVX2: %s\n", btoa(query_cpu(7, 1, 5)));
        log("AVX512: %s\n", btoa(query_cpu(7, 1, 16)));
#ifdef __AVX__
        if (avx) {
            enable_avx();
            log("AVX enabled\n");
        }
#endif
    }
}