# Tools
AS = nasm -f elf32
CC = i386-elf-g++
LD = i386-elf-ld

# Flags
CFLAGS = -m32 -ffreestanding -fno-exceptions -fno-rtti
CFLAGS += -std=c++17 -Wall -Wextra -pedantic
CFLAGS += -O2 -fomit-frame-pointer
CFLAGS += -I src/kernel/include -I src/libc/include

LDFLAGS = -n -T linker.ld

# Source files and corresponding object files
kernel_asm_sources = $(shell find src/kernel/assembly -name *.asm)
kernel_asm_objects = $(patsubst src/kernel/assembly%.asm, build/kernel/%.o, $(kernel_asm_sources))

kernel_cpp_sources = $(shell find src/kernel/src -name *.cpp)
kernel_objects = $(patsubst src/kernel/src/%.cpp, build/kernel/%.o, $(kernel_cpp_sources))

libc_cpp_sources = $(shell find src/libc/src -name *.cpp)
libc_objects = $(patsubst src/libc/src/%.cpp, build/libc/%.o, $(libc_cpp_sources))

kernel_headers = $(shell find src/kernel/include -name *.hpp)
libc_headers = $(shell find src/libc/include -name *.hpp)

bootloader_obj = build/boot/boot.o

# C++ global constructors support
crti_obj = build/boot/crti.o
crtn_obj = build/boot/crtn.o
crtbegin_obj = $(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
crtend_obj = $(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)

all_objects = $(bootloader_obj) $(kernel_asm_objects) $(kernel_objects) $(libc_objects)
obj_link_list = $(crti_obj) $(crtbegin_obj) $(all_objects) $(crtend_obj) $(crtn_obj)

image_file = build/image.bin

# Targets
build: $(image_file) count_sectors

$(bootloader_obj) $(crti_obj) $(crtn_obj): build/boot/%.o : src/boot/%.asm
	mkdir -p $(dir $@) && \
	$(AS) -o $@ $^

$(kernel_asm_objects): build/kernel/%.o : src/kernel/assembly/%.asm
	mkdir -p $(dir $@) && \
	$(AS) $(patsubst build/kernel/%.o, src/kernel/assembly/%.asm, $@) -o $@

$(kernel_objects): build/kernel/%.o : src/kernel/src/%.cpp $(kernel_headers)
	$(CC) -c -o $@ $< $(CFLAGS)

$(libc_objects): build/libc/%.o : src/libc/src/%.cpp $(libc_headers)
	mkdir -p $(dir $@) && \
	$(CC) -c -o $@ $< $(CFLAGS)

$(image_file): $(obj_link_list)
	$(LD) -o $@ $(obj_link_list) $(LDFLAGS)

.PHONY: run clean count_sectors

run: build
	qemu-system-x86_64 -no-reboot -drive format=raw,file=$(image_file)

count_sectors: $(image_file)
	@printf "\nSize of image.bin in sectors: "
	@echo "scale=4; `wc -c $(image_file) | sed 's/[^0-9]//g'` / 512" | bc -lq
	@printf "Currently loaded by bootloader: "
	@cat src/boot/include/sectors.asm | sed 's/[^0-9]//g'

clean:
	rm -rf build/*
