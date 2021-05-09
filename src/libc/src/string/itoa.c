#include <string.h>

char *itoa_rec(long num, char *buffer, int base) {
    if (num > 0) {
        buffer = itoa_rec(num / base, buffer, base);
        char digit = num % base;
        *buffer++ = digit <= 9 ? ('0' + digit) : ('a' + (digit - 10));
    }
    return buffer;
}

char *itoa(long num, char *buffer, int base) {
    if (num < 0) {
        *buffer++ = '-';
        num *= -1;
    }

    if (base == 16) {
        *buffer++ = '0';
        *buffer++ = 'x';
    }

    if (num == 0) {
        *buffer++ = '0';
    } else {
        buffer = itoa_rec(num, buffer, base);
    }

    *buffer = '\0';
    return buffer;
}