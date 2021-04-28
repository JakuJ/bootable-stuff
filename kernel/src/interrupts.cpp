#include "../include/PIC.hpp"
#include "../include/vga.hpp"

#define KBD_DATA_PORT       0x60
#define KBD_SCANCODE_MASK   0x7f
#define KBD_STATUS_MASK     0x80

VGA vga;

extern "C" {
void irq0_handler(void) {
    PIC_sendEOI(0);
}

void irq1_handler(void) {
    unsigned char scancode = inb(KBD_DATA_PORT);
    PIC_sendEOI(1);

    int released = scancode & KBD_STATUS_MASK;
    if (!released) {
        vga.print("Scan code: '", static_cast<unsigned>(scancode & KBD_SCANCODE_MASK), "'\n");
    }
}

void irq2_handler(void) {
    PIC_sendEOI(2);
}

void irq3_handler(void) {
    PIC_sendEOI(3);
}

void irq4_handler(void) {
    PIC_sendEOI(4);
}

void irq5_handler(void) {
    PIC_sendEOI(5);
}

void irq6_handler(void) {
    PIC_sendEOI(6);
}

void irq7_handler(void) {
    PIC_sendEOI(7);
}

void irq8_handler(void) {
    PIC_sendEOI(8);
}

void irq9_handler(void) {
    PIC_sendEOI(9);
}

void irq10_handler(void) {
    PIC_sendEOI(10);
}

void irq11_handler(void) {
    PIC_sendEOI(11);
}

void irq12_handler(void) {
    PIC_sendEOI(12);
}

void irq13_handler(void) {
    PIC_sendEOI(13);
}

void irq14_handler(void) {
    PIC_sendEOI(14);
}

void irq15_handler(void) {
    PIC_sendEOI(15);
}
}