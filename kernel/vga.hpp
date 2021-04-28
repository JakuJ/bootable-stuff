#pragma once

#include "utility.hpp"
#include "stdlib/type_traits.hpp"

class VGA {
    static constexpr unsigned TT_COLUMNS = 80;
    static constexpr unsigned TT_ROWS = 25;
    static short *const FRAMEBUFFER;

    short color = 0x1f00;

    unsigned cursorX = 0;
    unsigned cursorY = 0;

    void ensureCursorInRange();

public:
    void clearScreen();

    void putChar(const char c);

    void putStr(const char *str);

    template<typename T>
    typename std::enable_if<std::is_integer<T>::value>::type
    print(T num) {
        putChar('0' + (char) num);
    }

    template<typename T>
    typename std::enable_if<!std::is_integer<T>::value>::type
    print(T);

    template<typename T, typename ...Args>
    void printf(T t, Args... args) {
        printf(t);
        printf(args...);
    }

    template<typename T>
    void printf(T t) {
        print(t);
    }
};
