; isr.S - 32-bit protected mode ISR/IRQ stubs
BITS 32
global isr_common_stub
extern irq_handler_common

; Each IRQ stub pushes nothing special and calls the common C handler
; We'll make two simple IRQ stubs for IRQ0 and IRQ1

; IRQ0 -> int 32
global irq0_handler
irq0_handler:
    pusha
    push dword 0
    call irq_handler_common
    add esp, 4
    popa
    iret

; IRQ1 -> int 33
global irq1_handler
irq1_handler:
    pusha
    push dword 1
    call irq_handler_common
    add esp, 4
    popa
    iret
