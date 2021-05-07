#pragma once

#define pow(x, y) _Generic((x), double: dpow, int: ipow)(x, y)

int ipow(int value, int power) __attribute__((const));

double dpow(double value, int power) __attribute__((const));