#pragma once

#include "../macro_foreach.hpp"

namespace std {

    struct false_type {
        static const bool value = false;
    };

    struct true_type {
        static const bool value = true;
    };

    template<typename>
    struct is_integral : public false_type {
    };

#define GEN_INTEGRAL(type) template<> struct is_integral<type> : public true_type { };

    FOR_EACH(GEN_INTEGRAL, short, int, unsigned, long, unsigned long, long long, unsigned long long)

    template<bool, typename T = void>
    struct enable_if {
    };

    template<typename T>
    struct enable_if<true, T> {
        typedef T type;
    };

    template<bool B, class T = void>
    using enable_if_t = typename enable_if<B, T>::type;
}