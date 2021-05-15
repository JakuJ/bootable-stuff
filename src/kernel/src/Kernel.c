#include <VGA/VGA.h>
#include <Interrupts.h>
#include <PIC.h>
#include <IDT.h>
#include <Diagnostics.h>
#include <memory/PMM.h>
#include <memory/VMM.h>

// Kernel entry point
void kmain() {
    // Initialize resources
    pmm_init();
    vmm_init();
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

    // Do not exit from kernel, rather wait for interrupts
    while (true) {
        asm ("hlt");
    }
}
