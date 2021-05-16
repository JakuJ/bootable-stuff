#include <VGA/VGA.h>
#include <Interrupts.h>
#include <PIC.h>
#include <IDT.h>
#include <Diagnostics.h>
#include <KbController.h>
#include <Ring3.h>

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

    enable_interrupts();

    log("Entering ring 3...\n");

    enter_user_mode();

    log("Back from ring 3!\n");

    subscribePress(keyPressed);
    log("\nKeyboard input:\n");

    // Do not exit from kernel, rather wait for interrupts
    while (true) {
        asm ("hlt");
    }
}
