.PHONY=all clean

all: boot.o

boot.o: stage1.asm
	nasm -fbin stage1.asm -o boot.o

clean:
	rm -f *.o
