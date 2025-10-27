#include <stdint.h>
#include "ports.h"

extern void kprint(const char *s);
extern void pic_send_eoi(unsigned char irq);

/* Simple scancode -> ascii for common keys (partial map) */
static const char scancode_map[128] = {
0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
'\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,
'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\','z','x',
'c','v','b','n','m',',','.','/',0,'*',0,' ',0
};

void keyboard_handler() {
    uint8_t scancode = inb(0x60);
    if (scancode & 0x80) {
        // key released (ignore)
    } else {
        char c = scancode_map[scancode];
        if (c) {
            char s[2] = { c, 0 };
            kprint("KEY: ");
            kprint(s);
            kprint("\n");
        } else {
            kprint("KEY_SC:");
            // print scancode as hex
            char hex[8];
            const char *hexchars = "0123456789ABCDEF";
            hex[0] = hexchars[(scancode >> 4) & 0xF];
            hex[1] = hexchars[scancode & 0xF];
            hex[2] = 0;
            kprint(hex);
            kprint("\n");
        }
    }
    pic_send_eoi(1);
}
