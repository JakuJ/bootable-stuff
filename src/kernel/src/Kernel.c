#include <VGA.h>
#include <Interrupts.h>
#include <PIC.h>
#include <IDT.h>
#include <KbController.h>
#include <Diagnostics.h>
#include <string.h>
#include <memory/PMM.h>
#include <memory/VMM.h>

// Kernel entry point
extern void kmain() {
    // Initialize resources
    PIC_remap(0x20, 0x28);
    IDT_init();

    // Welcome user
    clearScreen();
    log("Kernel loaded\n\n");

    // Print section info
    print_sections();
    log("\n");

    log("Interrupts enabled: %s\n", btoa(are_interrupts_enabled()));

    // Test page allocation
    vmm_init();

    for (int i = 0; i < 62; i++) {
        pmm_allocate_page();
    }
    for (int i = 0; i < 5; i++) {
        void *page = pmm_allocate_page();
        log("Free page at: %p (%lu)\n", page, (unsigned long) page);
    }

    // Do not exit from kernel, rather wait for interrupts
    while (true) {
        asm ("hlt");
    }
}
