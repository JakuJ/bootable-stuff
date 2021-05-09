#include <memory/VMM.h>

int liballoc_lock() {
    return 0; // TODO
}

int liballoc_unlock() {
    return 0; // TODO
}

/** This is the hook into the local system which allocates pages. It
 * accepts an integer parameter which is the number of pages
 * required.  The page size was set up in the liballoc_init function.
 *
 * \return NULL if the pages were not allocated.
 * \return A pointer to the allocated memory.
 */
void *liballoc_alloc(int pages) {
    return vmm_allocate_pages(pages);
}

/** This frees previously allocated memory. The void* parameter passed
 * to the function is the exact same value returned from a previous
 * liballoc_alloc call.
 *
 * The integer value is the number of pages to free.
 *
 * \return 0 if the memory was successfully freed.
 */
int liballoc_free(void *start, int pages) {
    vmm_free_pages(start, pages);
    return 0; // TODO
}