#include <string.h>
#include <math.h>

char *gcvt(double value, int ndigits, char *buf) {
    if (value < 0) {
        *buf++ = '-';
        value *= -1;
    }

    // print decimal part
    int dec = (int) value;
    buf = itoa(dec, buf, 10);

    // print fractional part
    *buf++ = '.';
    double frac = value - dec;

    char tmp[ndigits];
    itoa(frac * pow(10.0, ndigits), tmp, 10);

    unsigned zeros = ndigits - strlen(tmp);
    while (zeros--) {
        *buf++ = '0';
    }

    return strcpy(buf, tmp);
}
