#pragma once

namespace std {

    struct false_type {
        static const bool value = false;
    };

    struct true_type {
        static const bool value = true;
    };

    template<typename>
    struct __is_integral_helper : public false_type {
    };

#define GEN_INTEGER(type) template<> struct __is_integral_helper<type> : public true_type { };

    GEN_INTEGER(short)
    GEN_INTEGER(int)
    GEN_INTEGER(unsigned)
    GEN_INTEGER(long)
    GEN_INTEGER(unsigned long)
    GEN_INTEGER(long long)
    GEN_INTEGER(unsigned long long)

    template<typename T>
    struct is_integer : public __is_integral_helper<T> {
    };

    template<bool, typename _Tp = void>
    struct enable_if {
    };

    template<typename _Tp>
    struct enable_if<true, _Tp> {
        typedef _Tp type;
    };
}