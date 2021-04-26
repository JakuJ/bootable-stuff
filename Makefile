.PHONY: clean

boot.bin: boot.asm
	nasm -f bin -o boot.bin boot.asm

run: boot.bin
	qemu-system-x86_64 -drive format=raw,file=boot.bin

clean:
	rm boot.bin
