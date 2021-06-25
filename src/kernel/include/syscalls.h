#pragma once

#include <stddef.h>
#include <stdarg.h>

#define ssize_t int

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
