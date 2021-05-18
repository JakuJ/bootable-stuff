#include <lib/string.h>

void vsprintf(char *buffer, char const *fmt, va_list arg) {
    char ch;

    while ((ch = *fmt++)) {
        if ('%' == ch) {
            switch (ch = *fmt++) {
                case '%': {
                    *buffer++ = '%';
                    break;
                }
                case 'c': {
                    *buffer++ = (char)va_arg(arg, int);
                    break;
                }
                case 's': {
                    char *str = va_arg(arg, char * );
                    buffer = strcpy(buffer, str);
                    break;
                }
                case 'l': {
                    switch (ch = *fmt++) {
                        case 'u': {
                            buffer = itoa(va_arg(arg, unsigned long), buffer, 10);
                            break;
                        }
                        case 'd': {
                            buffer = itoa(va_arg(arg, long), buffer, 10);
                            break;
                        }
                        case 'x': {
                            buffer = itoa(va_arg(arg, unsigned long), buffer, 16);
                            break;
                        }
                    }
                    break;
                }
                case 'u': {
                    buffer = itoa(va_arg(arg, unsigned int), buffer, 10);
                    break;
                }
                case 'd': {
                    buffer = itoa(va_arg(arg, int), buffer, 10);
                    break;
                }
                case 'p': {
                    buffer = itoa(va_arg(arg, unsigned long), buffer, 16);
                    break;
                }
                case 'x': {
                    buffer = itoa(va_arg(arg, unsigned int), buffer, 16);
                    break;
                }
                case 'f': {
                    buffer = gcvt(va_arg(arg, double), 5, buffer);
                    break;
                }
            }
        } else {
            *buffer++ = ch;
        }
    }
    *buffer = '\0';
}
