// Simple Phase 1 kernel
void kernel_main(void) {
    const char *msg = "Hello from MyOS-AI Kernel Phase 1!";
    char *vga = (char*)0xb8000; // VGA text buffer
    for (int i = 0; msg[i] != '\0'; i++) {
        vga[i*2] = msg[i];      // character
        vga[i*2 + 1] = 0x0F;    // color (white on black)
    }
    for (;;) {} // Infinite loop to keep kernel running
}
