#--------------------------------+
#				 |
#	     Alta OS		 |
#				 |
#  Makes boot sector and kernel	 |
#       Stanislav Podolsky	 |
#				 |
#--------------------------------+

CC=gcc
CFLAGS=-c -ffreestanding -Wall -std=gnu99 -nostdlib -nodefaultlibs -nostartfiles -I../include -O0

KERNEL_DIR=../kernel
BOOT_DIR=../boot
IMAGE_DIR=../image
TOOLS_DIR=../tools

.PHONY=all clean boot kernel tools update

########################################################
# 		      Entry point
########################################################

all: boot tools kernel

boot: 
	make -C $(BOOT_DIR) all

kernel:
	make -C $(KERNEL_DIR) all

tools:
	make -C $(TOOLS_DIR) all

############################
# Clean rule
############################

clean:

	make -C $(BOOT_DIR) clean
	make -C $(KERNEL_DIR) clean
	make -C $(TOOLS_DIR) clean

update: clean all

