cat > Makefile << 'EOF'
TARGET = myos
ISO = $(TARGET).iso

CC = gcc
LD = ld
AS = nasm
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-builtin
LDFLAGS = -m elf_i386

KERNEL_OBJS = kernel/kernel.o kernel/idt.o kernel/irq.o kernel/timer.o kernel/keyboard.o kernel/kprint.o
ISR_OBJ = kernel/isr.o
BOOT_OBJ = boot/boot.o

all: $(ISO)

boot/boot.o: boot/boot.asm
	$(AS) -f elf32 $< -o $@

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

kernel/isr.o: kernel/isr.S
	$(AS) -f elf32 kernel/isr.S -o kernel/isr.o

$(TARGET).bin: $(BOOT_OBJ) $(KERNEL_OBJS) $(ISR_OBJ)
	$(LD) $(LDFLAGS) -T boot/linker.ld -o $@ $^

$(ISO): $(TARGET).bin
	mkdir -p iso/boot/grub
	cp $(TARGET).bin iso/boot/$(TARGET).bin
	printf 'set timeout=0\nset default=0\nmenuentry "MyOS-AI" { multiboot /boot/$(TARGET).bin; boot }' > iso/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) iso

run: all
	qemu-system-x86_64 -cdrom $(ISO) -m 512M -serial stdio -monitor none

clean:
	rm -rf *.o *.bin iso $(KERNEL_OBJS) $(ISR_OBJ) $(BOOT_OBJ)
EOF
