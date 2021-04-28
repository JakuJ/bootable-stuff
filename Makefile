.PHONY: clean

bootloader.o: $(shell find bootloader -type f)
	nasm -f elf32 -o bootloader.o bootloader/bootloader.asm

image.bin: $(shell find kernel -type f) bootloader.o
	i386-elf-gcc -m32 kernel/*.cpp bootloader.o -o image.bin -nostdlib -ffreestanding -std=c++17 -mno-red-zone -fno-exceptions -nostdlib -fno-rtti -O2 -Wall -Wextra -T linking.ld

run: image.bin
	qemu-system-x86_64 -drive format=raw,file=image.bin

clean:
	rm -f *.bin *.o
