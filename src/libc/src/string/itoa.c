#include <string.h>

char *itoa_rec(int num, char *buffer, int base) {
    if (num > 0) {
        buffer = itoa_rec(num / base, buffer, base);
        int digit = num % base;
        *buffer++ = digit <= 9 ? ('0' + digit) : ('a' + (digit - 10));
    }
    return buffer;
}

char *itoa(int num, char *buffer, int base) {
    if (num == 0) {
        *buffer++ = '0';
    } else if (num < 0) {
        *buffer++ = '-';
        num *= -1;
    }

    if (num > 0) {
        buffer = itoa_rec(num, buffer, base);
    }

    *buffer = '\0';
    return buffer;
}
