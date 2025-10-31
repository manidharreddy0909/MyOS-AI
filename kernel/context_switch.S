[BITS 32]
global context_switch

section .text

; void context_switch(uint32_t **prev_esp, uint32_t *next_esp)
; Saves current ESP to *prev_esp, loads new ESP from next_esp

context_switch:
    mov eax, [esp + 4]     ; Get address of prev_esp (first argument)
    mov ebx, [esp + 8]     ; Get address of next_esp (second argument)
    mov [eax], esp         ; Save current stack pointer into *prev_esp
    mov esp, [ebx]         ; Load new stack pointer from *next_esp
    ret
