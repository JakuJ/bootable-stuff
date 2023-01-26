#pragma once

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>

#define ssize_t int
#define off_t long

typedef struct {
    void *iov_base;    /* Starting address */
    size_t iov_len;    /* Number of bytes to transfer */
} iovec;

typedef struct {
    unsigned short ws_row;
    unsigned short ws_col;
    unsigned short ws_xpixel;   /* unused */
    unsigned short ws_ypixel;   /* unused */
} winsize;

ssize_t write(int fd, const void *buf, size_t count);

int ioctl(int fd, int request, ...);

ssize_t writev(int fd, const iovec *iov, int iovcnt);

int open(const char *pathname, int flags);

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);

int brk(void *addr);
