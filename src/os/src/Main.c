#include <syscall.h>

void os_main(void) {
    syscall(0xdead);
    syscall(0xbabe);
}