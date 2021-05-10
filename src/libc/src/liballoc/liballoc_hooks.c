#include <memory/VMM.h>
#include <stdbool.h>

static inline bool atomic_compare_exchange(volatile int *ptr, int compare, int exchange) {
    return __atomic_compare_exchange_n(ptr, &compare, exchange, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

static inline void atomic_store(volatile int *ptr, int value) {
    __atomic_store_n(ptr, value, __ATOMIC_SEQ_CST);
}

volatile int lock = 0;

int liballoc_lock() {
    while (!atomic_compare_exchange(&lock, 0, 1)) {}
    return 0;
}

int liballoc_unlock() {
    atomic_store(&lock, 0);
    return 0;
}

void *liballoc_alloc(int pages) {
    return vmm_allocate_pages(pages);
}

int liballoc_free(void *start, size_t pages) {
    vmm_free_pages(start, pages);
    return 0; // TODO
}