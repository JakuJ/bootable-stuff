#include "../include/assertions.hpp"
#include "../include/vga.hpp"
#include "../../stdlib/type_traits.hpp"

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
    vga.print(-12, '*', 42u, '-', 1l, '=', (short) -505, '\n');
    vga.print(3.1415f, '*', -12.56, '=', -39.45724, '\n');
    vga.print(true);
}
