#include <VGA/VGA.h>
#include <Interrupts.h>
#include <PIC.h>
#include <IDT.h>
#include <Diagnostics.h>
#include <KbController.h>

void keyPressed(char c, unsigned char code) {
    if (c) {
        log("%c", c);
    } else {
        switch (code) {
            case 28:
                log("\n");
                break;
            case 14:
                log("\b");
                break;
            default:
                log("<%d>", code);
                break;
        }
    }
}

// Kernel entry point
void kmain() {
    // Initialize resources
    vga_init();

    PIC_remap(0x20, 0x28);
    IDT_init();

    // Welcome user
    clearScreen();

    log("Kernel loaded\n\n");

    vga_info();
    log("\n");
    section_info();
    log("\n");
    sse_info();
    log("\n");

    subscribePress(keyPressed);
    log("Keyboard input:\n");

    enable_interrupts();

    // Do not exit from kernel, rather wait for interrupts
    while (true) {
        asm ("hlt");
    }
}
