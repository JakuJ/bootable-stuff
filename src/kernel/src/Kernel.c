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

    vmm_allocate_pages(256); // Allocate a megabyte
    void *page = vmm_allocate_pages(1); // Outside the 2MB linearly paged region
    log("Page at: %p (%ld)", page, (unsigned long) page);
    log(", value: %ld\n", *(uint64_t *) page); // Page fault

    // Do not exit from kernel, rather wait for interrupts
    while (true) {
        asm ("hlt");
    }
}
