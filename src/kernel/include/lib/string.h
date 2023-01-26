#pragma once

#include <stddef.h>
#include <stdarg.h>

size_t strlen(const char *str);

char *strcpy(char *to, const char *from);

char *itoa(long input, char *buffer, int base);

char *gcvt(double value, int ndigits, char *buf);

void vsprintf(char *buffer, char const *fmt, va_list args);

int strcmp(const char *str1, const char *str2);