#include <stdint.h>
#include "ports.h"

extern void kprint(const char *s);
extern void pic_send_eoi(unsigned char irq);
void scheduler_tick(void);
volatile uint32_t tick_count = 0;

void init_timer(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

void timer_handler() {
    tick_count++;
    /* minimal visible trace (kept short to avoid flooding) */
    if ((tick_count & 0x3F) == 0) { /* every 64 ticks print */
        kprint("tick ");
        /* small number print omitted for brevity */
    }
    /* Call scheduler tick for preemption */
    scheduler_tick();
    pic_send_eoi(0);
}
