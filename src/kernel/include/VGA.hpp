#pragma once

#include "type_traits.hpp"

class VGA {
    static short *const FRAMEBUFFER;

    short color = 0x1f00;

    const unsigned rowMin = 0;
    const unsigned rowMax = TT_ROWS;
    const unsigned colMin = 0;
    const unsigned colMax = TT_COLUMNS;

    unsigned cursorX = 0;
    unsigned cursorY = 0;

    void ensureCursorInRange();

public:
    static constexpr unsigned TT_COLUMNS = 80;
    static constexpr unsigned TT_ROWS = 25;

    VGA();

    VGA(unsigned minColumn, unsigned maxColumn, unsigned minRow, unsigned maxRow, short color = 0x1f00);

    void clearScreen();

    template<typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
    void print_impl(T num, std::dummy_type) {
        if (num < 0) {
            print('-');
            print(-num);
        } else if (num > 0) {
            print(num / 10);
            print<char>('0' + (num % 10));
        }
    }

    template<typename T, std::enable_if_t<std::is_floating<T>::value, bool> = true>
    void print_impl(T num, std::dummy_type) {
        if (num < 0) {
            print('-');
            print(-num);
        } else if (num > 0) {
            // print decimal part
            long dec = static_cast<long>(num);
            print(dec);
            // print fractional part
            print('.');
            T frac = num - dec;
            print<long>(frac * 100000); // TODO: actually respect precision
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
