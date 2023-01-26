#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "syscalls.h"

#define PAGE_SIZE 0x1000
#define PROT_READ 0x1
#define PROT_WRITE 0x2
#define PROT_EXEC 0x4
#define PROT_NONE 0x8
#define MAP_SHARED 0x01

void *vmm_mmap(void* physical, size_t pages, bool is_kernel);

void vmm_free_pages(void *start, size_t pages);

void vmm_set_page_permissions(void);
