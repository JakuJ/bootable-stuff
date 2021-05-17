#include <VGA/VGA.h>

void handle_syscall(unsigned long code) {
    log("Got syscall %lx\n", code);
}