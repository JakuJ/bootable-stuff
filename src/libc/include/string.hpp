#pragma once

#include <stddef.h>
#include <stdarg.h>

size_t strlen(const char *str);

char *strcpy(char *to, char *from);

char *strcpy(char *to, const char *from);

char *itoa(int input, char *buffer, int base = 10);

char *gcvt(double value, int ndigits, char *buf);

void vsprintf(char *buffer, char const *fmt, va_list args);
