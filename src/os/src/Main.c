#include <stdio.h>
#include <unistd.h>
#include <string.h>

int os_main(void) {
    printf("Sending a syscall without a handler...\n\n");

    int ret = syscall(9, 10, 11, 12, 13, 14, 15); // TODO: long return type wraps around

    char buf[100] = {0};
    snprintf(buf, 100, "Got: %d from syscall 9\n\n", ret);

    write(1, buf, strlen(buf));

    printf("Fun fact: musl's [%s] uses [%s]\n", "printf", "the 'writev' syscall");

    printf("Use the keyboard to type. Press 1 to start the clock, 2 to stop it.\n\n");

    return 0;
}
