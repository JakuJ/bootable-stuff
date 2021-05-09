#include <VGA.h>
#include <Interrupts.h>
#include <PIC.h>
#include <IDT.h>
#include <KbController.h>
#include <Diagnostics.h>
#include <string.h>
#include <memory/VMM.h>
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

    // Test dynamic memory allocation
    vmm_init();

    size_t size = 1;
    char *array1 = kmalloc(size);
    log("kmalloc array of size %lu at %p\n", size, (void *) array1);

    size = 4096 * 20; // 20 pages
    char *array2 = kmalloc(size);
    log("kmalloc array of size %lu at %p\n", size, (void *) array2);

    size = 100;
    char *array3 = kmalloc(size);
    log("kmalloc array of size %lu at %p\n", size, (void *) array3);

    kfree(array2);
    kfree(array3);
    kfree(array1);

    // Do not exit from kernel, rather wait for interrupts
    while (true) {
        asm ("hlt");
    }
}
