#include <string.h>
#include <math.h>

char *gcvt(double value, int ndigits, char *buf) {
    if (value < 0) {
        *buf++ = '-';
        value *= -1;
    }

    // print decimal part
    int dec = (int)value;
    buf = itoa(dec, buf, 10);

    // print fractional part
    *buf++ = '.';
    double frac = value - dec;

    buf = itoa(frac * pow(10.0, ndigits), buf, 10);

    *buf = '\0';
    return buf;
}
