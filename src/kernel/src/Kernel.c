#include <VGA.h>
#include <Interrupts.h>
#include <PIC.h>
#include <IDT.h>
#include <KbController.h>
#include <Diagnostics.h>
#include <string.h>
#include <liballoc_1_1.h>

// Kernel entry point
void kmain() {
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

    // Test dynamic memory allocation
    for (int j = 0; j < 1; j++) {
        void *mems[2];
        for (int i = 0; i < 2; i++) {
            mems[i] = kmalloc(16 * 0x1000); // 10 MB
        }
        for (int i = 0; i < 2; i++) {
            kfree(mems[i]);
        }
    }

    // Do not exit from kernel, rather wait for interrupts
    while (true) {
        asm ("hlt");
    }
}
