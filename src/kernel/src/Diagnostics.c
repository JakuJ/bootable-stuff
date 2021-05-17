#include <Diagnostics.h>
#include <VGA/VGA.h>

#define btoa(x) ((x) ? "true" : "false")

extern int query_cpu(unsigned int page, int reg_no, int bit);

extern void enable_avx(void);

extern int linker_first_free_page;

void section_info(void) {

#define PRINT_SECTION(X) \
    extern int _##X##_START_, _##X##_END_; \
    log("\t" #X ":\t%lu bytes\t%lu - %lu\t(%p - %p)\n", \
    (unsigned long)&_##X##_END_ - (unsigned long)&_##X##_START_, \
    (unsigned long)(&_##X##_START_), (unsigned long)(&_##X##_END_), (void*)&_##X##_START_, (void*)&_##X##_END_);

    log("Image section sizes:\n");
    PRINT_SECTION(BOOT)
    log("\n");
    PRINT_SECTION(TEXT)
    PRINT_SECTION(DATA)
    log("\n");
    PRINT_SECTION(OS_TEXT)
    PRINT_SECTION(OS_DATA)
    log("\n");
    PRINT_SECTION(BSS)
    PRINT_SECTION(OS_BSS)
    log("\n\tFirst free page: %p\n", (void *) &linker_first_free_page);
}

void sse_info(void) {
    log("SSE support:\n");
    log("\tSSE3:\t%s\n", btoa(query_cpu(1, 2, 0)));
    log("\tSSSE3:\t%s\n", btoa(query_cpu(1, 2, 9)));
    log("\tSSE4.1:\t%s\n", btoa(query_cpu(1, 2, 19)));
    log("\tSSE4.2:\t%s\n", btoa(query_cpu(1, 2, 20)));
    log("\tSSE4a:\t%s\n", btoa(query_cpu(0x80000001, 2, 6)));

    int xsave = query_cpu(1, 2, 26);
    log("\tXSAVE:\t%s\n", btoa(xsave));

    if (xsave) {
        int avx = query_cpu(1, 2, 28);
        log("\tAVX:\t%s\n", btoa(avx));
        log("\tAVX2:\t%s\n", btoa(query_cpu(7, 1, 5)));
        log("\tAVX512:\t%s\n", btoa(query_cpu(7, 1, 16)));
#ifdef __AVX__
        if (avx) {
            enable_avx();
            log("AVX enabled\n");
        }
#endif
    }
}