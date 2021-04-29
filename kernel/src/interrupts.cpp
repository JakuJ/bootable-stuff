#include "../include/PIC.hpp"
#include "../include/port_io.hpp"
#include "../include/vga.hpp"

#define BLANK_IRQ(X) void irq##X##_handler(void) { PIC::sendEOI(X); }

#define KBD_DATA_PORT       0x60
#define KBD_SCANCODE_MASK   0x7f
#define KBD_STATUS_MASK     0x80

VGA vga;

extern "C" {

void irq1_handler(void) {
    unsigned char scancode = inb(KBD_DATA_PORT);
    PIC::sendEOI(1);

    int released = scancode & KBD_STATUS_MASK;
    if (!released) {
        vga.print("Scan code: '", static_cast<unsigned>(scancode & KBD_SCANCODE_MASK), "'\n");
    }
}

FOR_EACH(BLANK_IRQ, 0, 2, 3, 4, 5, 6, 7, 8)
FOR_EACH(BLANK_IRQ, 9, 10, 11, 12, 13, 14, 15)

}