#include <VGA/VGA.h>
#include <Interrupts.h>
#include <PIC.h>
#include <IDT.h>
#include <Diagnostics.h>
#include <string.h>
#include <liballoc_1_1.h>
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

    // Print section info
    print_sections();

    // Do not exit from kernel, rather wait for interrupts
    while (true) {
        asm ("hlt");
    }
}
