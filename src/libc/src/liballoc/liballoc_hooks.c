#include <memory/VMM.h>

// There is only one thread, and it's the kernel
int liballoc_lock() {
    return 0;
}

int liballoc_unlock() {
    return 0;
}

void *liballoc_alloc(int pages) {
    return vmm_allocate_pages(pages);
}

int liballoc_free(void *start, size_t pages) {
    vmm_free_pages(start, pages);
    return 0; // TODO: Can this even fail?
}