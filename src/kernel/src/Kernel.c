#include <VGA.h>
#include <Interrupts.h>
#include <PIC.h>
#include <IDT.h>
#include <KbController.h>
#include <Diagnostics.h>
#include <string.h>

// Kernel entry point
extern void kmain() {
    // Initialize resources
    PIC_remap(0x20, 0x28);
    IDT_init();
    VGA vga = VGA_init((VGA) {0});

    // Welcome user
    clearScreen(&vga);
    printf(&vga, "Kernel loaded\n\n");

    // Print section info
    print_sections(&vga);
    vga.cursorY++;

    // Print information on SSE extensions
    print_sse(&vga);

    printf(&vga, "Interrupts enabled: %s\n", btoa(are_interrupts_enabled()));

    // Do not exit from kernel, rather wait for interrupts
    while (true) {
        asm ("hlt");
    }
}
