#include <stdio.h>
#include <unistd.h>

int os_main(void) {
    int ret = syscall(9, 10, 11, 12, 13, 14, 15); // TODO: long return type wraps around

    char buf[100] = {0};
    snprintf(buf, 100, "Got: %d from syscall 9\n", ret);

    write(1, buf, 100);

    printf("Hello from %s's printf!!!\n", "musl"); // TODO: Add \n at the front and investigate buffering
    return 0;
}