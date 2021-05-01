#include <PIC.hpp>
#include <PortIO.hpp>
#include <KbController.hpp>
#include <VGA.hpp>
#include <interrupts.hpp>
#include <macro_foreach.hpp>

#define KBD_DATA_PORT       0x60
#define KBD_SCANCODE_MASK   0x7f
#define KBD_STATUS_MASK     0x80

bool are_interrupts_enabled() {
    unsigned long flags;
    asm volatile ("pushf\n\t pop %0" : "=g"(flags));
    return flags & (1 << 9);
}

extern "C" {

void isr_handler(const ISR_Frame regs) {
    static VGA vga(0, VGA::TT_COLUMNS, 0, VGA::TT_ROWS, 0x0400);
    switch (regs.int_no) {
        case 6: // Invalid opcode
            vga.printf("Invalid opcode\n");
            vga.printf("Instruction pointer: %d", regs.ip);
            break;
        case 13:
            vga.printf("General Protection Fault\n");
            if (regs.err_code != 0) {
                if (regs.err_code & 1) {
                    vga.printf("Exception of external origin");
                } else {
                    const char *sources[] = {"GDT", "IDT", "LDT", "IDT"};
                    vga.printf("Source: %s\n", sources[(regs.err_code >> 1) & 0x3]);
                    vga.printf("Selector index: %d", (regs.err_code >> 3) & 0x1ffff);
                }
            }
            break;
        default:
            vga.printf("Unhandled interrupt: %d", regs.int_no);
            break;
    }
    while (true);
}

void irq0_handler(void) {
    static long long counter = 0;
    static VGA clock_vga(0, VGA::TT_COLUMNS, 11, VGA::TT_ROWS);

    PIC::sendEOI(0);
    clock_vga.printf("Clock: %d\r", counter++);
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

#define BLANK_IRQ(X) void irq##X##_handler(void) { PIC::sendEOI(X); }

FOR_EACH(BLANK_IRQ, 2 , 3 , 4 , 5 , 6 , 7 , 8 )
FOR_EACH(BLANK_IRQ, 9 , 10 , 11 , 12 , 13 , 14 , 15 )

}