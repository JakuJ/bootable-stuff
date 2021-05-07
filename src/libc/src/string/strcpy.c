#include <string.h>

char *strcpy(char *to, const char *from) {
    while (*from) {
        *to++ = *from++;
    }
    *to = '\0';
    return to;
}
