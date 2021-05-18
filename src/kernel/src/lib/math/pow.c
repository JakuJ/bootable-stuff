#include <lib/math.h>

int ipow(int value, int power) {
    if (power <= 0) return 1;

    int ret = value;
    for (int i = 1; i < power; i++) {
        ret *= value;
    }

    return ret;
}

double dpow(double value, int power) {
    if (power <= 0) return 1.0;

    double ret = value;
    for (int i = 1; i < power; i++) {
        ret *= value;
    }

    return ret;
}