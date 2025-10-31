# ==== Compiler and Assembler settings ====
CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-builtin -Ikernel
LDFLAGS = -m elf_i386
TARGET = myos-ai
ISO = $(TARGET).iso

BOOT_OBJ = boot/boot.o
KERNEL_OBJS = kernel/kernel.o kernel/idt.o kernel/irq.o kernel/timer.o kernel/keyboard.o kernel/kprint.o kernel/memory.o kernel/sched.o kernel/context_switch.o
ISR_OBJ = kernel/isr.o
IDT_FLUSH_OBJ = kernel/idt_flush.o

# ==== Build rules ====
all: $(TARGET).bin

boot/boot.o: boot/boot.asm
	$(AS) -f elf32 boot/boot.asm -o boot/boot.o

kernel/kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel/idt.o: kernel/idt.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel/irq.o: kernel/irq.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel/timer.o: kernel/timer.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel/keyboard.o: kernel/keyboard.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel/kprint.o: kernel/kprint.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel/memory.o: kernel/memory.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel/sched.o: kernel/sched.c
	$(CC) $(CFLAGS) -c $< -o $@

# ✅ FIXED: use NASM for Intel syntax assembly
kernel/context_switch.o: kernel/context_switch.S
	nasm -f elf32 kernel/context_switch.S -o kernel/context_switch.o

kernel/isr.o: kernel/isr.S
	$(AS) -f elf32 kernel/isr.S -o kernel/isr.o

kernel/idt_flush.o: kernel/idt_flush.asm
	$(AS) -f elf32 kernel/idt_flush.asm -o kernel/idt_flush.o

# ==== Link everything ====
$(TARGET).bin: $(BOOT_OBJ) $(KERNEL_OBJS) $(ISR_OBJ) $(IDT_FLUSH_OBJ)
	$(LD) $(LDFLAGS) -T boot/linker.ld -o $@ $^

# ==== Create bootable ISO ====
$(ISO): $(TARGET).bin
	mkdir -p iso/boot/grub
	cp $(TARGET).bin iso/boot/$(TARGET).bin
	printf 'set timeout=0\nset default=0\nmenuentry "MyOS-AI" {\n  multiboot /boot/$(TARGET).bin\n  boot\n}\n' > iso/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) iso

# ==== Run with QEMU ====
run: all
	qemu-system-x86_64 -cdrom $(ISO) -m 512M -serial stdio -monitor none

# ==== Clean ====
clean:
	rm -rf *.o *.bin iso $(KERNEL_OBJS) $(ISR_OBJ) $(BOOT_OBJ) $(IDT_FLUSH_OBJ)
