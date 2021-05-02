#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble!"
#endif

//#if !defined(__i386__)
//#error "This kernel needs to be compiled with an x86-elf compiler!"
//#endif

#include <VGA.hpp>
#include <interrupts.hpp>
#include <PIC.hpp>
#include <IDT64.hpp>
#include <KbController.hpp>
#include <function.hpp>
#include <string.hpp>

extern "C" int query_cpu(int page, int reg_no, int bit);
extern "C" void enable_avx(void);

// Kernel entry point
extern "C" void kmain() {
    // Initialize resources
    PIC::remap(0x20, 0x28);
    IDT::init();
    VGA vga;

    // Welcome user
    vga.clearScreen();
    vga.printf("Kernel loaded\n\n");

    // Print information on SSE extensions
    vga.printf("MMX: %b\n", query_cpu(1, 3, 23));
    vga.printf("SSE: %b\n", query_cpu(1, 3, 25));
    vga.printf("SSE2: %b\n", query_cpu(1, 3, 26));
    vga.printf("SSE3: %b\n", query_cpu(1, 2, 0));
    vga.printf("SSSE3: %b\n", query_cpu(1, 2, 9));
    vga.printf("SSE4.1: %b\n", query_cpu(1, 2, 19));
    vga.printf("SSE4.2: %b\n", query_cpu(1, 2, 20));
    vga.printf("SSE4a: %b\n", query_cpu(0x80000001, 2, 6));

    int xsave = query_cpu(1, 2, 26);
    vga.printf("XSAVE: %b\n", xsave);

    if (xsave) {
        int avx = query_cpu(1, 2, 28);
        vga.printf("AVX: %b\n", avx);
        vga.printf("AVX2: %b\n", query_cpu(7, 1, 5));
        vga.printf("AVX512: %b\n", query_cpu(7, 1, 16));
        if (avx) {
            enable_avx();
        }
    }

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

    // TODO: Page fault handling
    // Provoke a page fault over 2MB
    // uint64_t addr = 0x1ffff0;
    // while (true) {
    //     uint32_t *ptr = reinterpret_cast<uint32_t *>(addr);
    //     vga.printf("Byte at addr 0x%x (%d): 0x%x\r", addr, addr, *ptr);
    //     addr++;
    // }

    // Do not exit from kernel, rather wait for interrupts
    while (true) {
        asm ("hlt");
    }
}
