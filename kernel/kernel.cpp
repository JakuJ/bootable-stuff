#include "assertions.hpp"
#include "vga.hpp"
#include "stdlib/type_traits.hpp"

// Kernel entry point
extern "C" void kmain() {
    // Initialize resources
    VGA vga;

    // Welcome user
    vga.clearScreen();
    vga.putStr("Kernel loaded\n\n");

    // Run diagnostics
    chackAssertions();
    vga.putStr("Diagnostics passed\n\n");

    // Test printf
    vga.printf("Hello, ", 12, '*', 42u, '-', 1l, '=', (short) 503);
}
