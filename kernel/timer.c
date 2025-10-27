#include <stdint.h>
#include "ports.h"

extern void kprint(const char *s);
extern void pic_send_eoi(unsigned char irq);

volatile uint32_t tick_count = 0;

/* Timer initialization for PIT - IRQ0 */
void init_timer(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;
    outb(0x43, 0x36);               // Command port
    outb(0x40, divisor & 0xFF);     // Low byte
    outb(0x40, (divisor >> 8) & 0xFF); // High byte
}

/* Called from irq handler with irq = 0 */
void timer_handler() {
    tick_count++;
    // Update tiny on-screen counter (fast)
    // Convert tick_count to ascii (simple)
    char buf[32];
    int n = 0;
    uint32_t v = tick_count;
    if (v == 0) buf[n++] = '0';
    else {
        char tmp[16];
        int t = 0;
        while (v) { tmp[t++] = '0' + (v % 10); v /= 10; }
        while (t--) buf[n++] = tmp[t];
    }
    buf[n] = '\0';
    kprint(" TICKS: ");
    kprint(buf);
    kprint("\n");
    pic_send_eoi(0);
}
