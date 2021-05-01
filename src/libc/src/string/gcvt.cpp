#include <string.hpp>
#include <math.hpp>

char *gcvt(double value, int ndigits, char *buf) {
    if (value < 0) {
        *buf++ = '-';
        value *= -1;
    }

    // print decimal part
    int dec = static_cast<int>(value);
    buf = itoa(dec, buf, 10);

    // print fractional part
    *buf++ = '.';
    double frac = value - dec;

//    buf = itoa(frac * pow(10.0, ndigits), buf);

    *buf = '\0';
    return buf;
}
