#pragma once

#include "VGA.hpp"
#include "../../stdlib/function.hpp"

using Handler = std::function<void, char, unsigned char> *;

class KbController {
    static constexpr int MAX_HANDLERS = 16;
    static constexpr int MAX_SCANCODES = 88;

    // TODO: Dynamic memory allocation
    static Handler pressHandlers[MAX_HANDLERS];
    static Handler releaseHandlers[MAX_HANDLERS];

    static char translationTable[MAX_SCANCODES];
    static char scanCodeToChar(unsigned char);

public:
    static bool subscribePress(Handler h);

    static bool subscribeRelease(Handler h);

    // TODO: These are only used by the interrupt handler itself
    // Maybe encapsulate and befriend it?
    static void onKeyPressed(unsigned char scancode);

    static void onKeyReleased(unsigned char scancode);
};
