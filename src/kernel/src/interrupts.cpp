#include "PIC.hpp"
#include "port_io.hpp"
#include "KbController.hpp"

#define BLANK_IRQ(X) void irq##X##_handler(void) { PIC::sendEOI(X); }

#define KBD_DATA_PORT       0x60
#define KBD_SCANCODE_MASK   0x7f
#define KBD_STATUS_MASK     0x80

extern "C" {

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

FOR_EACH(BLANK_IRQ, 0, 2, 3, 4, 5, 6, 7, 8)
FOR_EACH(BLANK_IRQ, 9, 10, 11, 12, 13, 14, 15)

}