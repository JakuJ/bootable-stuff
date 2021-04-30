kernel_asm_sources = $(shell find src/kernel/assembly -name *.asm)
kernel_asm_object_files = $(patsubst src/kernel/assembly%.asm, build/kernel/%.o, $(kernel_asm_sources))

kernel_cpp_sources = $(shell find src/kernel/src -name *.cpp)
kernel_object_files = $(patsubst src/kernel/src/%.cpp, build/kernel/%.o, $(kernel_cpp_sources))

bootloader_object_file = build/bootloader.o
image_file = build/image.bin

# TODO: vga.print(1, 2, 3, 4, 5, 6, 7, 8, 9);
# with the x86_64-elf-g++ compiler
# produce interrupt 6 (invalid opcode)

CC = i386-elf-g++
LD = i386-elf-ld

CFLAGS = -nostdlib -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti
CFLAGS += -O2 -m32 -std=c++17 -Wall -Wextra
CFLAGS += -I src/kernel/include -I src/stdlib

LDFLAGS = -n -T linking.ld

all: $(image_file) count_sectors

$(bootloader_object_file): src/bootloader/bootloader.asm
	mkdir -p $(dir $@) && \
	nasm -f elf32 -o $@ $^

$(kernel_asm_object_files): build/kernel/%.o : src/kernel/assembly/%.asm
	mkdir -p $(dir $@) && \
	nasm -f elf32 $(patsubst build/kernel/%.o, src/kernel/assembly/%.asm, $@) -o $@

$(kernel_object_files): build/kernel/%.o : src/kernel/src/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(image_file): $(bootloader_object_file) $(kernel_asm_object_files) $(kernel_object_files)
	$(LD) -o $@ $^ $(LDFLAGS)

.PHONY: run clean count_sectors

run: all
	qemu-system-x86_64 -no-reboot -drive format=raw,file=$(image_file)

count_sectors: $(image_file)
	@printf "\nSize of image.bin in sectors: "
	@echo "scale=4; `wc -c $(image_file) | sed 's/[^0-9]//g'` / 512" | bc -lq
	@printf "Currently loaded in bootloader: "
	@cat src/bootloader/sectors.asm | sed 's/[^0-9]//g'

clean:
	rm -f *.bin *.o
	rm -rf build/*
