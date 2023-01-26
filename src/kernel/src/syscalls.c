#include <syscalls.h>
#include <lib/syscall.h.in>
#include <VGA/VGA.h>
#include <lib/string.h>
#include <lib/liballoc_1_1.h>
#include <memory/VMM.h>

extern vbe_screen_info vbe_info;
extern uintptr_t FRAMEBUFFER;

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

int brk(void *addr) {
    // TODO: Process table and memory management
    return -1;
}

int open(__attribute((unused)) const char *pathname, __attribute((unused)) int flags) {
    // TODO: File system, only memory mapping supported for now

    // pathname has to be "/dev/mem"
    if (strcmp(pathname, "/dev/mem") != 0) {
        log("open: pathname must be \"/dev/mem\", got \"%s\"\n", pathname);
        return -1;
    }

    return 0xdeadbeef;
}

void *mmap(__attribute((unused)) void *addr, size_t length, __attribute((unused)) int prot,
           __attribute((unused)) int flags, int fd, off_t offset) {
    // TODO: this is not actually mmap, more like physical memory aliasing
    size_t pages = length / PAGE_SIZE;

    if (fd == -1 && addr == NULL) {
        // TODO: is this always the case for malloc?
        addr = vmm_mmap(0, pages, false);
        return addr + offset;
    }

    if ((uintptr_t) addr % PAGE_SIZE != 0) {
        log("[VMM] mmap: addr must be page-aligned, got %x\n", addr);
        return (void *) -1;
    }

    // TODO: Access control my ass
    if (fd != 0xdeadbeef) {
        log("[VMM] mmap: fd must be -1 (malloc) or 0xdeadbeef (\"/dev/mem\"), got %x\n", fd);
        return (void *) -1;
    }

    void *physical = vbe_info.physical_buffer & ~0xfff;

    if (addr == NULL) {
        addr = vmm_mmap(physical, pages, false);
    }

    return (void *) (addr + offset);
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
        case __NR_open:
            log("SYSCALL: [open] %s\n", (const char *) a);
            return open((const char *) a, (int) b);
        case __NR_mmap:
            log("SYSCALL: [mmap] %ld bytes at %lx with offset %ld of file %ld\n", b, a, f, e);
            return (long) mmap((void *) a, (size_t) b, (int) c, (int) d, (int) e, f);
        case __NR_brk:
            log("SYSCALL: [brk] %lx\n", a);
            return brk((void *) a);
        default:
            log("UNHANDLED SYSCALL %ld with args %ld %ld %ld %ld %ld %ld\n", n, a, b, c, d, e, f);
            return -1;
    }
}