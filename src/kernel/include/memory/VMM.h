#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define PAGE_SIZE 0x1000

void vmm_init(void);

void *vmm_allocate_pages(size_t pages);

void vmm_free_pages(void *start, size_t pages);

void vmm_map_memory(uintptr_t physical, uintptr_t virtual);
