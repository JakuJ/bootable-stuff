#include <VGA/VGA.h>
#include <Interrupts.h>
#include <PIC.h>
#include <IDT.h>
#include <Diagnostics.h>
#include <Ring3.h>
#include <stdnoreturn.h>
#include <memory/VMM.h>
#include <KbController.h>

void printKey(char character, __attribute((unused)) unsigned char scancode) {
    switch (character) {
        case '1':
            IRQ_clear_mask(0);
            return;
        case '2':
            IRQ_set_mask(0);
            return;
        default:
            log("%c", character);
            break;
    }
}

noreturn void kmain() {
    // Initialize resources
    vga_init();

    PIC_remap(0x20, 0x28);
    IDT_init();

    // Welcome user
    clearScreen();

    vga_info();
    log("\n");
    section_info();

    vmm_set_page_permissions();
    subscribePress(printKey);

    // Disable the clock by default
    IRQ_set_mask(0);
    enable_interrupts();

    log("\nEntering user space...\n");
    enter_user_mode();

    log("Back from ring 3 somehow ?!\n");

    // Do not exit from kernel, rather wait for interrupts
    while (true) {
        asm ("hlt");
    }
}
