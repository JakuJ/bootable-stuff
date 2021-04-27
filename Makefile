.PHONY: clean

bootloader.o: bootloader.asm
	nasm -f elf32 -o bootloader.o bootloader.asm

image.bin: kernel.cpp bootloader.o
	i386-elf-gcc -m32 kernel.cpp bootloader.o -o image.bin -nostdlib -ffreestanding -std=c++11 -mno-red-zone -fno-exceptions -nostdlib -fno-rtti -Wall -Wextra -Werror -T linking.ld

run: image.bin
	qemu-system-x86_64 -drive format=raw,file=image.bin

clean:
	rm -f *.bin *.o
