#pragma once

#include "VGA.hpp"
#include "../../stdlib/function.hpp"

// A handler is a pointer to a gen_function
using Handler = std::function<void, unsigned char> *;

class KbController {
    static constexpr int MAX_HANDLERS = 16;

    // TODO: Dynamic memory allocation
    static Handler pressHandlers[MAX_HANDLERS];
    static Handler releaseHandlers[MAX_HANDLERS];

public:
    static bool subscribePress(Handler h);

    static bool subscribeRelease(Handler h);

    // TODO: These are only used by the interrupt handler itself
    // Maybe encapsulate and befriend it?
    static void onKeyPressed(unsigned char scancode);

    static void onKeyReleased(unsigned char scancode);
};
