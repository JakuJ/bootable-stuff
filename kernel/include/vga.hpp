#pragma once

#include "utility.hpp"
#include "../../stdlib/type_traits.hpp"

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

    template<typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
    void print_impl(T num, std::dummy_type) {
        if (num < 0) {
            print('-');
            print(-num);
        } else if (num > 0) {
            print(num / 10);
            putChar('0' + static_cast<char>(num % 10));
        }
    }

    template<typename T, std::enable_if_t<std::is_floating<T>::value, bool> = true>
    void print_impl(T num, std::dummy_type) {
        if (num < 0) {
            putChar('-');
            print(-num);
        } else if (num > 0) {
            // print decimal part
            long dec = static_cast<long>(num);
            print(dec);
            // print fractional part
            putChar('.');
            T frac = num - dec;
            print(static_cast<long>(frac * 100000)); // arbitrary
        }
    }

    template<typename T>
    void print_impl(T, ...);

    template<typename T, typename ...Args>
    void print(T t, Args... args) {
        print(t);
        print(args...);
    }

    template<typename T>
    void print(T t) {
        print_impl(t, std::dummy_type());
    }
};
