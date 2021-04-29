#include "../include/KbController.hpp"

Handler KbController::pressHandlers[] = {nullptr};
Handler KbController::releaseHandlers[] = {nullptr};

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
            (*pressHandlers[i])(scancode);
        }
    }
}

void KbController::onKeyReleased(unsigned char scancode) {
    for (int i = 0; i < MAX_HANDLERS; i++) {
        if (releaseHandlers[i]) {
            (*releaseHandlers[i])(scancode);
        }
    }
}