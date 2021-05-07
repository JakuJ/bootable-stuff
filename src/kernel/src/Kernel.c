#include <VGA.h>
#include <Interrupts.h>
#include <PIC.h>
#include <IDT.h>
#include <KbController.h>
#include <string.h>
#include <Diagnostics.h>

// Kernel entry point
extern void kmain() {
    // Initialize resources
    PIC_remap(0x20, 0x28);
    IDT_init();
    VGA vga = {
        .rowMax = TT_ROWS,
        .colMax = TT_COLUMNS,
        .color = WHITE_ON_BLUE,
    };

    // Welcome user
    clearScreen(&vga);
    printf(&vga, "Kernel loaded\n\n");

    // Print section info
    print_sections(&vga);

    // Print information on SSE extensions
    print_sse(&vga);

    printf(&vga, "Interrupts enabled: %b\n", are_interrupts_enabled());

    // Do not exit from kernel, rather wait for interrupts
    while (true) {
        asm ("hlt");
    }
}
