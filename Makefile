.PHONY: clean

bootloader.o: $(shell find bootloader -type f)
	nasm -f elf32 -o bootloader.o bootloader/bootloader.asm

idt.o: bootloader/idt.asm
	nasm -f elf32 -o idt.o bootloader/idt.asm

image.bin: $(shell find kernel -type f) bootloader.o idt.o
	i386-elf-gcc -O2 -m32 kernel/src/*.cpp bootloader.o idt.o -o image.bin -nostdlib -ffreestanding -std=c++17 -mno-red-zone -fno-exceptions -nostdlib -fno-rtti -Wall -Wextra -T linking.ld
	@printf "\nSize of image.bin in sectors: "
	@echo "scale=4; `wc -c image.bin | sed 's/[^0-9]//g'` / 512" | bc -lq
	@printf "Currently loaded in bootloader: "
	@grep -E "number of sectors to read" bootloader/bootloader.asm | sed 's/[^0-9]//g'

run: image.bin
	qemu-system-x86_64 -no-reboot -drive format=raw,file=image.bin

clean:
	rm -f *.bin *.o
