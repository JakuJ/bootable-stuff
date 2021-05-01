#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble!"
#endif

//#if !defined(__i386__)
//#error "This kernel needs to be compiled with an x86-elf compiler!"
//#endif

#include <VGA.hpp>
#include <interrupts.hpp>
#include <PIC.hpp>
//#include <IDT.hpp>
#include <KbController.hpp>
#include <function.hpp>
#include <string.hpp>

// Kernel entry point
extern "C" void kmain() {
    // Initialize resources
    PIC::remap(0x20, 0x28);
//    IDT::init();
    VGA vga;

    // Welcome user
    vga.clearScreen();
    vga.printf("Kernel loaded\n\n");

    // Run diagnostics
    vga.printf("VGA printing test:\n");

    vga.printf("Integral types: %d * 0x%x - %d = %d\n", -12, 42u, 1l, -505);

//    vga.printf("FP types: %f * %f = %f\n", 3.1415f, -12.56, -39.45724);
    vga.printf("Booleans: %b, %b\n\n", true, false);

    vga.printf("Interrupts enabled: %b\n", are_interrupts_enabled());
    // Keyboard event handler factory
    auto mk_handler = [](VGA &v) {
        return [&v](char symbol, unsigned char scancode) {
            if (symbol != 0) {
                v.putChar(symbol);
            } else {
                v.printf("<%d>", scancode);
            }
        };
    };

    // Register key pressed handler
    VGA kb_press_vga(40, VGA::TT_COLUMNS, 0, 12);
    kb_press_vga.printf("Keyboard scancodes (pressed):\n");

    auto press_callback = std::make_function(mk_handler(kb_press_vga));
    auto *pointer = reinterpret_cast<std::function<void, char, unsigned char> *>(&press_callback);
    bool success = KbController::subscribePress(pointer);
    vga.printf("Keyboard press handler registered: %b\n", success);

    // Register key released handler
    VGA kb_release_vga(40, VGA::TT_COLUMNS, 13, VGA::TT_ROWS);
    kb_release_vga.printf("Keyboard scancodes (released):\n");

    auto release_callback = std::make_function(mk_handler(kb_release_vga));
    pointer = dynamic_cast<std::function<void, char, unsigned char> *>(&release_callback);
    success = KbController::subscribeRelease(pointer);
    vga.printf("Keyboard release handler registered: %b\n", success);

    // TODO: We are passing pointers to stack-allocated objects :)

    // Do not exit from kernel, rather wait for interrupts
    while (true) {
        asm ("hlt");
    }
}
