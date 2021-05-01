#include <string.hpp>

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
                    *buffer++ = static_cast<char>(va_arg(arg, int));
                    break;
                }
                case 's': {
                    char *str = va_arg(arg, char * );
                    buffer = strcpy(buffer, str);
                    break;
                }
                case 'd': {
                    int tmp = va_arg(arg, int);
                    buffer = itoa(tmp, buffer, 10);
                    break;
                }
                case 'x': {
                    int tmp = va_arg(arg, int);
                    buffer = itoa(tmp, buffer, 16);
                    break;
                }
                case 'f': {
//                    double tmp = va_arg(arg, double);
//                    buffer = gcvt(tmp, 5, buffer); // TODO: Specifying precision
                    break;
                }
                case 'b': {
                    int tmp = va_arg(arg, int);
                    buffer = strcpy(buffer, tmp ? "true" : "false");
                }
            }
        } else {
            *buffer++ = ch;
        }
    }
    *buffer = '\0';
}
