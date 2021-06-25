#include <syscalls.h>
#include <lib/syscall.h.in>
#include <VGA/VGA.h>

extern vbe_screen_info vbe_info;

ssize_t write(__attribute((unused)) int fd, const void *buf, size_t count) {
    log_n((char *) buf, count);
    return (ssize_t) count;
}

int ioctl(__attribute((unused)) int fd, int request, ...) {
#define RET(x) va_end(arg); return (x);
    va_list arg;
    va_start(arg, request);

    switch (request) {
        case 0x5413: {
            winsize *out = va_arg(arg, winsize*);
            *out = (winsize) {
                    .ws_col = vbe_info.x_cur_max,
                    .ws_row = vbe_info.y_cur_max,
                    .ws_xpixel = vbe_info.width,
                    .ws_ypixel = vbe_info.height,
            };
        }
            RET(0)
        default:
            log("Unknown IOCTL request %x\n", request);
            RET(-1)
    }
#undef RET
}

ssize_t writev(int fd, const iovec *iov, int iovcnt) {
    ssize_t written = 0;
    for (int i = 0; i < iovcnt; i++) {
        write(fd, iov[i].iov_base, iov[i].iov_len);
        written += (ssize_t) iov[i].iov_len;
    }
    return written;
}

int handle_syscall(long a, long b, long c, long d, long e, long f, long n) {
    switch (n) {
        case __NR_write:
            log("SYSCALL: [write] %ld bytes to FD %ld\n", c, a);
            return write((int) a, (const void *) b, c);
        case __NR_ioctl:
            log("SYSCALL: [ioctl] for FD %ld, request %lx\n", a, b);
            return ioctl((int) a, (int) b, c);
        case __NR_writev:
            log("SYSCALL: [writev] %ld buffers to FD %ld\n", c, a);
            return writev((int) a, (const iovec *) b, (int) c);
        default:
            log("UNHANDLED SYSCALL %ld with args %ld %ld %ld %ld %ld %ld\n", n, a, b, c, d, e, f);
            return -1;
    }
}