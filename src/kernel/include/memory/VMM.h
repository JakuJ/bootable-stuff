#pragma once

#include <stddef.h>

void *vmm_allocate_pages(size_t pages);

void vmm_free_pages(void *start, size_t pages);
