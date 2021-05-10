#include <VGA.h>
#include <Interrupts.h>
#include <PIC.h>
#include <IDT.h>
#include <KbController.h>
#include <Diagnostics.h>
#include <string.h>
#include <liballoc_1_1.h>

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

    // Test dynamic memory allocation - calloc 80 MB of space
    void *mem = kcalloc(80, 0x100000);
    kfree(mem);

    const int num = 10;
    void *mems[num];
    for (int i = 0; i < num; i++) {
        mems[i] = kcalloc(10, 0x100000); // 10 MB
    }
    for (int i = 0; i < num; i++) {
        kfree(mems[i]);
    }

    // Do not exit from kernel, rather wait for interrupts
    while (true) {
        asm ("hlt");
    }
}
