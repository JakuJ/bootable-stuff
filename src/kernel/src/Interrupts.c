#include <PIC.h>
#include <PortIO.h>
#include <KbController.h>
#include <VGA.h>
#include <Interrupts.h>
#include <macros.h>

#define KBD_DATA_PORT       0x60
#define KBD_SCANCODE_MASK   0x7f
#define KBD_STATUS_MASK     0x80
#define NUM_EXCEPTIONS      21

bool are_interrupts_enabled() {
    unsigned long flags;
    asm volatile ("pushf\n pop %0" : "=g"(flags));
    return flags & (1 << 9);
}

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

extern void isr_handler(const ISR_Frame regs) {
    static VGA vga = {
            .rowMax = TT_ROWS,
            .colMax = TT_COLUMNS,
            .color = WHITE_ON_BLACK,
    };

    // Print exception type
    if (regs.int_no < NUM_EXCEPTIONS) {
        printf(&vga, "Exception: %s\n", exceptions[regs.int_no]);
    } else {
        printf(&vga, "Unknown exception: %d\n", regs.int_no);
    }

    // Special handing for some exceptions
    switch (regs.int_no) {
        case 13:
            if (regs.err_code != 0) {
                if (regs.err_code & 1) {
                    printf(&vga, "Exception of external origin\n");
                } else {
                    const char *sources[] = {"GDT", "IDT", "LDT", "IDT"};
                    printf(&vga, "Source: %s\n", sources[(regs.err_code >> 1) & 0x3]);
                    printf(&vga, "Selector index: %d\n", (regs.err_code >> 3) & 0x1ffff);
                }
            }
            break;
        case 14: {
            uint64_t address;

            // Faulting address is stored in CR2
            asm volatile ("mov %0, cr2" : "=r"(address));
            printf(&vga, "Address: %lx (%lu)\n", address, address);

            printf(&vga, regs.err_code & 1 ? "Page-protection violation\n" : "Non-present page\n");
            printf(&vga, regs.err_code & 2 ? "Write access\n" : "Read access\n");
            printf(&vga, "Caused by process in ring %d\n", (regs.err_code & 4) ? 3 : 0);
            if (regs.err_code & 8) {
                printf(&vga, "One or more page directory entries contain reserved bits which are set to 1\n");
            }
            if (regs.err_code & 16) {
                printf(&vga, "Caused by an instruction fetch\n");
            }
            break;
        }
    }

    // Register dump
    printf(&vga,
           "Registers:\nA: %lu | B: %lu | C: %lu | D: %lu\nDI: %lu | SI: %lu\nIP: %lu\nBP: %lu | SP: %lu\nCS: %lu | DS: %lu | SS: %lu\n",
           regs.ax, regs.bx, regs.cx, regs.dx, regs.di, regs.si, regs.ip, regs.bp, regs.sp, regs.cs, regs.ds, regs.ss);

    while (true);
}

extern void irq0_handler(void) {
    static unsigned long counter = 0;
    static VGA vga = {
            .rowMax = TT_ROWS,
            .colMin = 65,
            .cursorX = 65,
            .colMax = TT_COLUMNS,
            .color = WHITE_ON_BLUE,
    };

    PIC_send_EOI(0);
    printf(&vga, "Clock: %lu\n", counter++);
}

extern void irq1_handler(void) {
    unsigned char scancode = inb(KBD_DATA_PORT);
    PIC_send_EOI(1);

    int released = scancode & KBD_STATUS_MASK;
    unsigned char code = scancode & KBD_SCANCODE_MASK;

    if (!released) {
        onKeyPressed(code);
    } else {
        onKeyReleased(code);
    }
}

#define BLANK_IRQ(X) extern void irq##X##_handler(void) { PIC_send_EOI(X); }

FOR_EACH(BLANK_IRQ,
         2, 3, 4, 5, 6, 7, 8)

FOR_EACH(BLANK_IRQ,
         9, 10, 11, 12, 13, 14, 15)
