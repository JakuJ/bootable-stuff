# Tools
AS = nasm -f elf64
CC = x86_64-elf-gcc
LD = x86_64-elf-ld

# Flags
CFLAGS = -std=gnu11 -masm=intel
CFLAGS += -O3 -Wall -Wextra -pedantic -Wno-pointer-arith
CFLAGS += -ffreestanding -mmmx -msse -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2
CFLAGS += -I src/kernel/include -I src/libc/include

LDFLAGS = -n -T linker.ld

# Source files and corresponding object files
kernel_asm_sources = $(shell find src/kernel/assembly -maxdepth 1 -name *.asm)
kernel_asm_objects = $(patsubst src/kernel/assembly%.asm, build/kernel/%.o, $(kernel_asm_sources))

kernel_c_sources = $(shell find src/kernel/src -name *.c)
kernel_objects = $(patsubst src/kernel/src/%.c, build/kernel/%.o, $(kernel_c_sources))

libc_c_sources = $(shell find src/libc/src -name *.c)
libc_objects = $(patsubst src/libc/src/%.c, build/libc/%.o, $(libc_c_sources))

kernel_headers = $(shell find src/kernel/include -name *.h)
libc_headers = $(shell find src/libc/include -name *.h)

bootloader_sources = $(shell find src/boot -maxdepth 1 -name *.asm)
bootloader_objs = $(patsubst src/boot/%.asm, build/boot/%.o, $(bootloader_sources))

# Global constructor support
crti_obj = build/boot/crti.o
crtn_obj = build/boot/crtn.o
crtbegin_obj = $(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
crtend_obj = $(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)

bootloader_objs := $(filter-out $(crti_obj) $(crtn_obj),$(bootloader_objs))

all_objects = $(bootloader_objs) $(kernel_asm_objects) $(kernel_objects) $(libc_objects)
obj_link_list = $(crti_obj) $(crtbegin_obj) $(all_objects) $(crtend_obj) $(crtn_obj)

image_file = build/image.bin

# Targets
build: $(image_file) count_sectors

$(bootloader_objs) $(crti_obj) $(crtn_obj): build/boot/%.o : src/boot/%.asm
	mkdir -p $(dir $@) && \
	$(AS) -o $@ $^

$(kernel_asm_objects): build/kernel/%.o : src/kernel/assembly/%.asm
	mkdir -p $(dir $@) && \
	$(AS) $(patsubst build/kernel/%.o, src/kernel/assembly/%.asm, $@) -o $@

$(kernel_objects): build/kernel/%.o : src/kernel/src/%.c $(kernel_headers)
	mkdir -p $(dir $@) && \
	$(CC) -c -o $@ $< $(CFLAGS)

$(libc_objects): build/libc/%.o : src/libc/src/%.c $(libc_headers)
	mkdir -p $(dir $@) && \
	$(CC) -c -o $@ $< $(CFLAGS)

$(image_file): $(obj_link_list)
	$(LD) -o $@ $(obj_link_list) $(LDFLAGS)

.PHONY: qemu64 hvf clean count_sectors

qemu64: build
	qemu-system-x86_64 -no-reboot \
	-cpu qemu64,+mmx,+sse,+sse2,+sse3,+ssse3,+sse4a,+sse4.1,+sse4.2,+xsave \
	-drive format=raw,file=$(image_file)

hvf: build
	qemu-system-x86_64 \
	-M accel=hvf -cpu host,+mmx,+sse,+sse2,+sse3,+ssse3,+sse4.1,+sse4.2,+xsave,+avx,+avx2 \
	-drive format=raw,file=$(image_file)

count_sectors: $(image_file)
	@printf "\nSize of image.bin in sectors: "
	@echo "scale=4; `wc -c $(image_file) | sed 's/[^0-9]//g'` / 512" | bc -lq
	@printf "Currently loaded by bootloader: "
	@cat src/boot/include/sectors.asm | sed 's/[^0-9]//g'

clean:
	rm -rf build/*
