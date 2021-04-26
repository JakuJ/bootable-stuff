.PHONY: clean

hello.bin: hello.asm
	nasm -f bin -o hello.bin hello.asm

hello: hello.bin
	qemu-system-x86_64 -drive format=raw,file=hello.bin

clean:
	rm *.bin
