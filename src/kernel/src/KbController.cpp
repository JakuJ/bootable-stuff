#include "KbController.hpp"

Handler KbController::pressHandlers[] = {nullptr};
Handler KbController::releaseHandlers[] = {nullptr};
char KbController::translationTable[] = {0, 0,
                                         '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
                                         'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0,
                                         'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
                                         '~', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, 0, 0, ' '};

bool KbController::subscribePress(Handler h) {
    for (int i = 0; i < MAX_HANDLERS; i++) {
        if (!pressHandlers[i]) {
            pressHandlers[i] = h;
            return true;
        }
    }
    return false;
}

bool KbController::subscribeRelease(Handler h) {
    for (int i = 0; i < MAX_HANDLERS; i++) {
        if (!releaseHandlers[i]) {
            releaseHandlers[i] = h;
            return true;
        }
    }
    return false;
}

void KbController::onKeyPressed(unsigned char scancode) {
    for (int i = 0; i < MAX_HANDLERS; i++) {
        if (pressHandlers[i]) {
            (*pressHandlers[i])(scanCodeToChar(scancode), scancode);
        }
    }
}

void KbController::onKeyReleased(unsigned char scancode) {
    for (int i = 0; i < MAX_HANDLERS; i++) {
        if (releaseHandlers[i]) {
            (*releaseHandlers[i])(scanCodeToChar(scancode), scancode);
        }
    }
}

char KbController::scanCodeToChar(unsigned char scancode) {
    if (scancode < MAX_SCANCODES) {
        return translationTable[scancode];
    }
    return 0;
}