TARGET = myos
ISO = $(TARGET).iso

CC = gcc
LD = ld
AS = nasm
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -m elf_i386

OBJS = boot/boot.o kernel/kernel.o

all: $(ISO)

boot/boot.o: boot/boot.asm
	$(AS) -f elf32 $< -o $@

kernel/kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).bin: $(OBJS)
	$(LD) $(LDFLAGS) -T boot/linker.ld -o $@ $(OBJS)

$(ISO): $(TARGET).bin
	mkdir -p iso/boot/grub
	cp $(TARGET).bin iso/boot/$(TARGET).bin
	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo 'menuentry "MyOS-AI" {' >> iso/boot/grub/grub.cfg
	echo ' multiboot /boot/$(TARGET).bin' >> iso/boot/grub/grub.cfg
	echo ' boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) iso

run: all
	qemu-system-x86_64 -cdrom $(ISO) -m 512M -serial stdio -monitor none

clean:
	rm -rf *.o *.bin iso
