CC=gcc
CFLAGS=-Wall

.PHONY=all clean

all: create_kernel type_size

clean:
	rm create_kernel
	rm type_size

create_kernel:
	$(CC) create_kernel.c $(CFLAGS) -o create_kernel

type_size:
	$(CC) type_size.c $(CFLAGS) -o type_size

