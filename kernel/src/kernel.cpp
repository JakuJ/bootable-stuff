#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble!"
#endif

#if !defined(__i386__)
#error "This kernel needs to be compiled with a x86-elf compiler!"
#endif

#include "../include/assertions.hpp"
#include "../include/vga.hpp"
#include "../include/interrupts.hpp"
#include "../include/PIC.hpp"
#include "../include/IDT.hpp"

// Kernel entry point
extern "C" void kmain() {
    // Initialize resources
    VGA vga;

    // Welcome user
    vga.clearScreen();
    vga.print("Kernel loaded\n\n");

    // Run diagnostics
    chackAssertions();
    vga.print("Diagnostics passed\n\n");

    // Test printf
    vga.print("Printing test:\n");
    vga.print("Integral types: ", -12, '*', 42u, '-', 1l, '=', (short) -505, '\n');
    vga.print("FP types: ", 3.1415f, '*', -12.56, '=', -39.45724, '\n');
    vga.print("Booleans: ", true, ", ", false, "\n\n");

    // Test interrupts
    vga.print("Interrupts enabled: ", are_interrupts_enabled(), '\n');
    vga.print("Enabling...\n");

    PIC_remap(0x20, 0x28);
    idt_init();

    vga.print("Interrupts enabled: ", are_interrupts_enabled(), '\n');

    while (true) {
        asm ("hlt");
    }
}
