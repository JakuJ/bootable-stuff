#pragma once

#include <stddef.h>
#include <stdarg.h>

#define btoa(x) ((x) ? "true" : "false")

size_t strlen(const char *str);

char *strcpy(char *to, const char *from);

char *itoa(long input, char *buffer, int base);

char *gcvt(double value, int ndigits, char *buf);

void vsprintf(char *buffer, char const *fmt, va_list args);
