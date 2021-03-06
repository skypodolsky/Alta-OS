IMAGE_DIR=../image
BOOT_DIR=../boot

CFLAGS=-m32 -ffreestanding -Wall -std=gnu99 -nostdlib -nodefaultlibs -nostartfiles -I../include -Os
OBJECTS=kernel.o main.o tty_asm.o io.o stdlib.o gdt.o idt.o exceptions.o exceptions_api.o irq.o irq_api.o keyboard.o time.o mem.o time_api.o ata.o fs.o file.o math.o sched.o
KERNEL_IMG=alta_kernel.bin

.PHONY=all clean update

all: $(IMAGE_DIR)/$(KERNEL_IMG)

#########################################################
# Building all sources -> assembler sources + C sources #
#########################################################

$(IMAGE_DIR)/$(KERNEL_IMG): $(OBJECTS) $(BOOT_DIR)/boot.o

# implicit compiling of C sources

# Linking all together
	ld -melf_i386 -Ttext 0x0 -Tdata 0x400000 -o local_kernel $(OBJECTS)

# Stripping unneded
	strip --remove-section=.eh_frame local_kernel

# Converting to the binary format and copying into the $(IMAGE_DIR)
	ld -melf_i386 --oformat binary -Ttext 0x0 -Tdata 0x400000 local_kernel -o $(IMAGE_DIR)/kernel

	cp $(BOOT_DIR)/boot.o $(IMAGE_DIR)/
	cp $(BOOT_DIR)/boot.o $(IMAGE_DIR)/alta_bootloader.bin
	dd if=$(IMAGE_DIR)/kernel of=$(IMAGE_DIR)/boot.o seek=1
	mv $(IMAGE_DIR)/boot.o $(IMAGE_DIR)/$(KERNEL_IMG)
	dd if=/dev/zero of=$(IMAGE_DIR)/$(KERNEL_IMG) seek=128 count=4096

# Assembler sources
tty_asm.o: tty.asm
	nasm -felf $< -o $@

kernel.o: kernel.asm
	nasm -felf $< -o $@


clean:
	rm -f *.o
	rm -f local_kernel
	rm -f $(IMAGE_DIR)/alta_intact
	rm -f $(IMAGE_DIR)/$(KERNEL_IMG)
	rm -f $(IMAGE_DIR)/alta_bootloader.bin
	rm -f $(IMAGE_DIR)/kernel

