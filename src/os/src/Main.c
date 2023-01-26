#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "fcntl.h"
#include "malloc.h"
#include <stdnoreturn.h>
#include <sys/mman.h>
#include <stdint.h>

#define FB_SIZE (800 * 600 * 4)

noreturn void dvd_bounce() {
    // open memory "file"
    int fb = open("/dev/mem", 0);
    printf("/dev/mem \"file\" descriptor: %x\n", fb);

    // mmap framebuffer
    void *fb_ptr = mmap(NULL, FB_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0);

    // print that address in hex
    printf("Framebuffer address: %x\n", fb_ptr);

    // malloc a backbuffer of the same size
    void *original = malloc(FB_SIZE);
    memcpy(original, fb_ptr, FB_SIZE);

    void *backbuffer = malloc(FB_SIZE);

    // in a loop, write a square to screen and move it diagonally
    int x = 0, y = 0;
    int dx = 1, dy = 1;
    while (1) {
        // copy original to backbuffer
        memcpy(backbuffer, original, FB_SIZE);

        // draw a square
        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 100; j++) {
                ((uint32_t *) backbuffer)[(y + i) * 800 + (x + j)] = 0x0000FF;
            }
        }

        // copy backbuffer to screen
        memcpy(fb_ptr, backbuffer, FB_SIZE);

        // move the square
        x += dx;
        y += dy;

        // bounce off the edges
        if (x + 100 >= 800 || x <= 0) dx *= -1;
        if (y + 100 >= 600 || y <= 0) dy *= -1;

        // busy wait for a bit
        for (int i = 0; i < 5000000; i++) {
            asm volatile("nop");
        }
    }
}

noreturn void os_main(void) {
    printf("Sending a syscall without a handler...\n\n");

    int ret = syscall(123, 10, 11, 12, 13, 14, 15); // TODO: long return type wraps around

    char buf[100] = {0};
    snprintf(buf, 100, "Got: %d from syscall 9\n\n", ret);

    write(1, buf, strlen(buf));
    printf("Fun fact: musl's [%s] uses [%s]\n", "printf", "the 'writev' syscall");

    dvd_bounce();
}
