#include <VGA/VGA.h>
#include <Interrupts.h>
#include <PIC.h>
#include <IDT.h>
#include <Diagnostics.h>
#include <Ring3.h>
#include <stdnoreturn.h>
#include <memory/VMM.h>

noreturn void kmain() {
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

    log("\nEntering ring 3...\n");

    vmm_set_page_permissions();
    enable_interrupts();
    enter_user_mode();

    log("Back from ring 3 somehow!\n");

    // Do not exit from kernel, rather wait for interrupts
    while (true) {
        asm ("hlt");
    }
}
