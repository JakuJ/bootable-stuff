#include <VGA/VGA.h>
#include <stddef.h>

typedef struct {
    void *iov_base;    /* Starting address */
    size_t iov_len;    /* Number of bytes to transfer */
} iovec;

int handle_syscall(long a, long b, long c, long d, long e, long f, long n) {
    switch (n) {
        case 1: // write
            log("CALL: [write] %ld bytes to FD %ld\n", c, a);
            log("%s", (char *) b); // TODO: Actually respect the length
            return 0;
        case 16:
            log("CALL: [ioctl] for FD %ld, request %lx\n", a, b);
            return 0;
        case 20: {
            log("CALL: [writev] to FD %ld\n", a);
            iovec *buffers = (iovec *) b;
            size_t written = 0;
            for (long i = 0; i < c; i++) {
                log("%s", (char *) buffers[i].iov_base);
                written += buffers[i].iov_len; // TODO: Actually respect the length
            }
            return written;
        }
        default:
            log("CALL: %ld with args %ld %ld %ld %ld %ld %ld\n", n, a, b, c, d, e, f);
            return -1;
    }
}