#include "PIC.hpp"
#include "PortIO.hpp"
#include "KbController.hpp"
#include "VGA.hpp"
#include "interrupts.hpp"

#define BLANK_IRQ(X) void irq##X##_handler(void) { PIC::sendEOI(X); }

#define KBD_DATA_PORT       0x60
#define KBD_SCANCODE_MASK   0x7f
#define KBD_STATUS_MASK     0x80

bool are_interrupts_enabled() {
    unsigned long flags;
    asm volatile ("pushf\n\t pop %0" : "=g"(flags));
    return flags & (1 << 9);
}

extern "C" {

void isr_handler(Registers regs) {
    static VGA vga(0, VGA::TT_COLUMNS, 0, VGA::TT_ROWS, 0x0400);
    PIC::sendEOI(regs.int_no);

    switch (regs.int_no) {
        case 6: // Invalid opcode
            vga.print("Invalid opcode\n");
            vga.print("Instruction pointer: ", regs.eip);
            break;
        case 13:
            vga.print("General Protection Fault\n");
            if (regs.err_code != 0) {
                if (regs.err_code & 1) {
                    vga.print("Exception of external origin");
                } else {
                    const char *sources[] = {"GDT", "IDT", "LDT", "IDT"};
                    vga.print("Source: ", sources[(regs.err_code >> 1) & 0x3], '\n');
                    vga.print("Selector index: ", (regs.err_code >> 3) & 0x1ffff);
                }
            }
            break;
        default:
            vga.print("Unhandled interrupt: ", regs.int_no, '\n');
            break;
    }
    while (true);
}


void irq1_handler(void) {
    unsigned char scancode = inb(KBD_DATA_PORT);
    PIC::sendEOI(1);

    int released = scancode & KBD_STATUS_MASK;
    unsigned char code = scancode & KBD_SCANCODE_MASK;

    if (!released) {
        KbController::onKeyPressed(code);
    } else {
        KbController::onKeyReleased(code);
    }
}

FOR_EACH(BLANK_IRQ, 0 , 2 , 3 , 4 , 5 , 6 , 7 , 8 )
FOR_EACH(BLANK_IRQ, 9 , 10 , 11 , 12 , 13 , 14 , 15 )

}