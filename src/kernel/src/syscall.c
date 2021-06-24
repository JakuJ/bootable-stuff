#include <VGA/VGA.h>

int handle_syscall(long a, long b, long c, long d, long e, long f) {
    switch (a) {
        case 1: // write
            log("%s", (char *) f);
            return 0;
        default:
            log("Got syscall with args %lx %lx %lx %lx %lx %lx\n", a, b, c, d, e, f);
            return -1;
    }
}