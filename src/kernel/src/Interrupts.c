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

            // Trace paging structures
            unsigned int pt4_index = ((unsigned long) address >> 39) & 0x1ff;
            unsigned int pt3_index = ((unsigned long) address >> 30) & 0x1ff;
            unsigned int pt2_index = ((unsigned long) address >> 21) & 0x1ff;
            unsigned int page_index = ((unsigned long) address >> 12) & 0x1ff;

            unsigned long *p4, *p3, *p2, *p1;
            asm ("mov %0, cr3" : "=r"(p4));
            printf(&vga, "PML4T at: %p\n", (void *) p4);
            printf(&vga, "PML4T entry %u : %lx\n", pt4_index, p4[pt4_index]);
            p3 = (unsigned long *) (p4[pt4_index] & ~0xfff);
            printf(&vga, "PDPT entry %u: %lx\n", pt3_index, p3[pt3_index]);
            p2 = (unsigned long *) (p3[pt3_index] & ~0xfff);
            printf(&vga, "PDT entry %u: %lx\n", pt2_index, p2[pt2_index]);
            p1 = (unsigned long *) (p2[pt2_index] & ~0xfff);
            printf(&vga, "PT entry %u: %lx\n", page_index, p1[page_index] & 0xffffffffffffUL);
            break;
        }
    }

    // Register dump
    printf(&vga,
           "Registers:\nA: %lx | B: %lx | C: %lx | D: %lx\nDI: %lx | SI: %lx\nIP: %lx\nBP: %lx | SP: %lx\nCS: %lx | DS: %lx | SS: %lx\n",
           regs.ax, regs.bx, regs.cx, regs.dx, regs.di, regs.si, regs.ip, regs.bp, regs.sp, regs.cs, regs.ds, regs.ss);

    while (true);
}

extern void irq0_handler(void) {
    static unsigned long counter = 0;
    static VGA vga = {
            .rowMax = TT_ROWS,
            .colMin = 67,
            .cursorX = 67,
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
