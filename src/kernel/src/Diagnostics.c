#include <Diagnostics.h>
#include <VGA/VGA.h>
#include <stdint.h>

#define btoa(x) ((x) ? "true" : "false")

extern int query_cpu(unsigned int page, int reg_no, int bit);

extern void enable_avx(void);

extern uintptr_t _FIRST_FREE_PAGE_[];

void section_info(void) {

#define PRINT_SECTION(X) \
    extern uintptr_t _##X##_START_[], _##X##_END_[]; \
    uintptr_t X##e = (uintptr_t)_##X##_END_, X##s = (uintptr_t) _##X##_START_; \
    log("\t" #X ":\t%lu bytes\t%lu - %lu\t(%lx - %lx)\n", X##e - X##s, X##s, X##e, X##s, X##e);

    log("Image section sizes:\n");
    PRINT_SECTION(BOOT)
    log("\n");
    PRINT_SECTION(KERNEL_TEXT)
    PRINT_SECTION(KERNEL_RODATA)
    PRINT_SECTION(KERNEL_DATA)
    log("\n");
    PRINT_SECTION(OS_TEXT)
    PRINT_SECTION(OS_RODATA)
    PRINT_SECTION(OS_DATA)
    log("\n");
    PRINT_SECTION(KERNEL_BSS)
    PRINT_SECTION(OS_BSS)

#undef PRINT_SECTION

    log("\n\tFirst free page at: %lx\n", (uintptr_t) _FIRST_FREE_PAGE_);
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