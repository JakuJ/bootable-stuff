#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble!"
#endif

#if !defined(__i386__)
#error "This kernel needs to be compiled with a x86-elf compiler!"
#endif

#include "../include/assertions.hpp"
#include "../include/VGA.hpp"
#include "../include/interrupts.hpp"
#include "../include/PIC.hpp"
#include "../include/IDT.hpp"
#include "../include/KbController.hpp"
#include "../../stdlib/function.hpp"

// Kernel entry point
extern "C" void kmain() {
    // Initialize resources
    PIC::remap(0x20, 0x28);
    IDT::init();
    VGA vga;

    // Welcome user
    vga.clearScreen();
    vga.print("Kernel loaded\n\n");

    // Run diagnostics
    chackAssertions();

    vga.print("VGA printing test:\n");
    vga.print("Integral types: ", -12, '*', 42u, '-', 1l, '=', (short) -505, '\n');
    vga.print("FP types: ", 3.1415f, '*', -12.56, '=', -39.45724, '\n');
    vga.print("Booleans: ", true, ", ", false, "\n\n");

    vga.print("Interrupts enabled: ", are_interrupts_enabled(), '\n');

    vga.print("Diagnostics passed\n\n");

    // TODO: We are passing pointer to stack-allocated objects :)

    // Register key pressed handler
    VGA kb_press_vga(40, VGA::TT_COLUMNS, 0, 12);
    kb_press_vga.print("Keyboard scancodes (pressed):\n");

    auto press_callback = [&](unsigned char scancode) {
        kb_press_vga.print(static_cast<unsigned>(scancode), ' ');
    };

    auto press_f = std::make_function(press_callback);
    auto *pc = reinterpret_cast<std::function<void, unsigned char> *>(&press_f);
    bool success = KbController::subscribePress(pc);
    vga.print("Keyboard press handler registered: ", success, '\n');

    // Register key released handler
    VGA kb_release_vga(40, VGA::TT_COLUMNS, 13, VGA::TT_ROWS);
    kb_release_vga.print("Keyboard scancodes (released):\n");

    auto release_callback = [&](unsigned char scancode) {
        kb_release_vga.print(static_cast<unsigned>(scancode), ' ');
    };

    auto release_f = std::make_function(release_callback);
    pc = reinterpret_cast<std::function<void, unsigned char> *>(&release_f);
    success = KbController::subscribeRelease(pc);
    vga.print("Keyboard release handler registered: ", success, '\n');

    // Do not exit from kernel, rather wait for interrupts
    while (true) {
        asm ("hlt");
    }
}
