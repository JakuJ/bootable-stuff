#include "assertions.hpp"
#include "vga.hpp"

// Kernel entry point
extern "C" void kmain() {
    // Initialize resources
    VGA vga;

    // Welcome user
    vga.clearScreen();
    vga.putStr("Kernel loaded\n");

    // Run diagnostics
    chackAssertions();
    vga.putStr("Diagnostics passed");
}
