.PHONY: clean

bootloader.bin: bootloader.asm
	nasm -f bin -o bootloader.bin bootloader.asm

kernel.bin: kernel.asm
	nasm -f bin -o kernel.bin kernel.asm

image.bin: bootloader.bin kernel.bin
	cat bootloader.bin kernel.bin > image.bin

run: image.bin
	qemu-system-x86_64 -drive format=raw,file=image.bin

clean:
	rm *.bin
