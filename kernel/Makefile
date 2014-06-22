IMAGE_DIR=../image
BOOT_DIR=../boot

CFLAGS=-m32 -ffreestanding -Wall -std=gnu99 -nostdlib -nodefaultlibs -nostartfiles -I../include -Os
OBJECTS=kernel.o main.o tty_asm.o io.o stdlib.o gdt.o idt.o exceptions.o exceptions_api.o irq.o irq_api.o keyboard.o time.o
KERNEL_IMG=alta_kernel.bin

.PHONY=all clean

all: $(IMAGE_DIR)/$(KERNEL_IMG)

#########################################################
# Building all sources -> assembler sources + C sources #
#########################################################

$(IMAGE_DIR)/$(KERNEL_IMG): $(OBJECTS)

# implicit compiling of C sources

# Linking all together
	ld -melf_i386 -Ttext 0x0 -Tdata 0x400000 -o local_kernel $(OBJECTS)

# Stripping unneded
	strip --remove-section=.eh_frame local_kernel

# Converting to the binary format and copying into the $(IMAGE_DIR)
	ld --oformat binary -Ttext 0x0 -Tdata 0x400000 local_kernel -o $(IMAGE_DIR)/kernel

	cp $(BOOT_DIR)/boot.o $(IMAGE_DIR)/
	dd if=$(IMAGE_DIR)/kernel of=$(IMAGE_DIR)/boot.o seek=1
	../tools/create_kernel $(IMAGE_DIR)/boot.o
	mv $(IMAGE_DIR)/boot.o $(IMAGE_DIR)/alta_intact
	mv $(IMAGE_DIR)/kernel $(IMAGE_DIR)/$(KERNEL_IMG)
	dd if=/dev/zero of=$(IMAGE_DIR)/$(KERNEL_IMG) seek=128 count=1

# Assembler sources
tty_asm.o: tty.asm
	nasm -felf $< -o $@

kernel.o: kernel.asm
	nasm -felf $< -o $@


clean:
	rm *.o
	rm local_kernel
	rm $(IMAGE_DIR)/alta_intact
	rm $(IMAGE_DIR)/$(KERNEL_IMG)
