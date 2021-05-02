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
    asm volatile ("pushf\n pop %0" : "=g"(flags));
    return flags & (1 << 9);
}

#define NUM_EXCEPTIONS 21
const char *exceptions[NUM_EXCEPTIONS] = {
        "Division By Zero",
        "Debug",
        "Non-maskable Interrupt",
        "Breakpoint",
        "Overflow",
        "Bound Range Exceeded",
        "Invalid Opcode",
        "Device Not Available",
        "Double Fault",
        "Coprocessor Segment Overrun",
        "Invalid TSS",
        "Segment Not Present",
        "Stack-Segment Fault",
        "General Protection Fault",
        "Page Fault",
        "Reserved",
        "x87 Floating-Point Exception",
        "Alignment Check",
        "Machine Check",
        "SIMD Floating-Point Exception",
        "Virtualization Exception",
};

extern "C" {

void isr_handler(const ISR_Frame regs) {
    static VGA vga(0, VGA::TT_COLUMNS, 0, VGA::TT_ROWS, 0x0f00);

    // Print exception type
    if (regs.int_no < NUM_EXCEPTIONS) {
        vga.printf("Exception: %s\n", exceptions[regs.int_no]);
    } else {
        vga.printf("Unknown exception: %d\n", regs.int_no);
    }

    // Special handing for some exceptions
    switch (regs.int_no) {
        case 13:
            if (regs.err_code != 0) {
                if (regs.err_code & 1) {
                    vga.printf("Exception of external origin\n");
                } else {
                    const char *sources[] = {"GDT", "IDT", "LDT", "IDT"};
                    vga.printf("Source: %s\n", sources[(regs.err_code >> 1) & 0x3]);
                    vga.printf("Selector index: %d\n", (regs.err_code >> 3) & 0x1ffff);
                }
            }
            break;
    }

    // Register dump
    vga.printf(
            "Registers:\nA: %d | B: %d | C: %d | D: %d\nDI: %d | SI: %d\nIP: %d\nBP: %d | SP: %d\nCS: %d | DS: %d | SS: %d\n",
            regs.ax, regs.bx, regs.cx, regs.dx, regs.di, regs.si, regs.ip, regs.bp, regs.sp, regs.cs, regs.ds, regs.ss);

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