#include <string.hpp>

char *strcpy(char *to, char *from) {
    while (*from) {
        *to++ = *from++;
    }
    *to = '\0';
    return to;
}

char *strcpy(char *to, const char *from) {
    return strcpy(to, const_cast<char *>(from));
}
