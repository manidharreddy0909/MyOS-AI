[bits 32]
[extern kernel_main]
global _start

_start:
    cli                 ; Disable interrupts
    mov esp, 0x90000    ; Set up stack
    call kernel_main     ; Jump to C kernel
.hang:
    hlt
    jmp .hang
