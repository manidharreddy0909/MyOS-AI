#include <stdint.h>
#include "idt.h"
#include "ports.h"

/* External ISR/IRQ stubs from isr.S */
extern void irq0_handler();
extern void irq1_handler();

/* C IRQ handlers */
void irq_handler_common(int irq);

/* From modules */
void init_idt();
void pic_remap(int, int);
void pic_enable_irq(int);
void init_timer(uint32_t);
void timer_handler();
void keyboard_handler();
void kprint(const char *);

void setup_idt_and_irqs() {
    // IDT already initialized in init_idt
    // set gates for IRQ0 (vector 32) and IRQ1 (33)
    // Here set gate 32 -> address of irq0_handler, gate 33 -> irq1_handler
    // We'll compute addresses in C using function pointers cast to uint32_t
    idt_set_gate(32, (uint32_t)irq0_handler, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t)irq1_handler, 0x08, 0x8E);
}

void irq_handler_common(int irq) {
    if (irq == 0) timer_handler();
    else if (irq == 1) keyboard_handler();
    // else ignore
}

void kernel_main(void) {
    // Clear screen banner
    kprint("\n\n=== MyOS-AI Phase2 Kernel Boot ===\n");
    init_idt();
    pic_remap(32, 40);         // remap PIC to vectors 32-47
    setup_idt_and_irqs();
    pic_enable_irq(0);        // enable timer
    pic_enable_irq(1);        // enable keyboard
    init_timer(100);          // 100 Hz

    kprint("Initialization complete. Timer and keyboard enabled.\n");
    kprint("Press any key to see scancode.\n");

    // idle loop
    for (;;) { __asm__ __volatile__("hlt"); }
}

