#include <stdint.h>

void *memset(void *dest, int val, unsigned int len) {
    unsigned char *ptr = dest;
    while (len-- > 0)
        *ptr++ = (unsigned char)val;
    return dest;
}
