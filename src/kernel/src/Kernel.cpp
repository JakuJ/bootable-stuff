#include <VGA.hpp>
#include <Interrupts.hpp>
#include <PIC.hpp>
#include <IDT.hpp>
#include <KbController.hpp>
#include <function.hpp>
#include <string.hpp>
#include <Diagnostics.hpp>

// Kernel entry point
extern "C" void kmain() {
    // Initialize resources
    PIC::remap(0x20, 0x28);
    IDT::init();
    VGA vga;

    // Welcome user
    vga.clearScreen();
    vga.printf("Kernel loaded\n\n");

    // Print section info
    print_sections(vga);

    // Print information on SSE extensions
    print_sse(vga);

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
    auto *pointer = dynamic_cast<std::function<void, char, unsigned char> *>(&press_callback);
    bool success = KbController::subscribePress(pointer);
    vga.printf("Keyboard press handler: %b\n", success);

    // Register key released handler
    VGA kb_release_vga(40, VGA::TT_COLUMNS, 13, VGA::TT_ROWS);
    kb_release_vga.printf("Keyboard scancodes (released):\n");

    auto release_callback = std::make_function(mk_handler(kb_release_vga));
    pointer = dynamic_cast<std::function<void, char, unsigned char> *>(&release_callback);
    success = KbController::subscribeRelease(pointer);
    vga.printf("Keyboard release handler: %b\n", success);

    // TODO: We are passing pointers to stack-allocated objects :)

    // Do not exit from kernel, rather wait for interrupts
    while (true) {
        asm ("hlt");
    }
}
