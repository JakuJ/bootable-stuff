#include <KbController.h>

#define MAX_HANDLERS 8

char translationTable[] = {0, 0,
                           '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
                           'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
                           'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
                           '~', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, 0, 0, ' '};

Handler pressHandlers[MAX_HANDLERS];
Handler releaseHandlers[MAX_HANDLERS];

static char scanCodeToChar(unsigned char scancode) {
    if (scancode < sizeof(translationTable)) {
        return translationTable[scancode];
    }
    return 0;
}

bool subscribePress(Handler h) {
    for (int i = 0; i < MAX_HANDLERS; i++) {
        if (!pressHandlers[i]) {
            pressHandlers[i] = h;
            return true;
        }
    }
    return false;
}

bool subscribeRelease(Handler h) {
    for (int i = 0; i < MAX_HANDLERS; i++) {
        if (!releaseHandlers[i]) {
            releaseHandlers[i] = h;
            return true;
        }
    }
    return false;
}

void onKeyPressed(unsigned char scancode) {
    for (int i = 0; i < MAX_HANDLERS; i++) {
        if (pressHandlers[i]) {
            (*pressHandlers[i])(scanCodeToChar(scancode), scancode);
        }
    }
}

void onKeyReleased(unsigned char scancode) {
    for (int i = 0; i < MAX_HANDLERS; i++) {
        if (releaseHandlers[i]) {
            (*releaseHandlers[i])(scanCodeToChar(scancode), scancode);
        }
    }
}
