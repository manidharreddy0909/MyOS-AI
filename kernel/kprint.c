#include <stdint.h>

/* Simple VGA & serial printing for kernel debugging */
static char *video = (char*)0xb8000;
static int curpos = 0;

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__ ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void kprint(const char *s) {
    // also send to serial COM1 (0x3F8)
    for (int i=0; s[i]; ++i) {
        // VGA
        video[curpos++] = s[i];
        video[curpos++] = 0x07;
        if (curpos >= 80*25*2) curpos = 0;
        // serial
        outb(0x3f8, s[i]);
    }
}
