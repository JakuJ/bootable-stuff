# Tools
AS = nasm -f elf64
CC = x86_64-elf-gcc
LD = x86_64-elf-ld
OBJCOPY = x86_64-elf-objcopy

# Constants
PORTS_DIR = ports

# Common flags
COM_FLAGS = -std=gnu18 -masm=intel
COM_FLAGS += -O3 -Wall -Wextra -Wpedantic -Wstrict-aliasing -fanalyzer
COM_FLAGS += -Wno-pointer-arith
COM_FLAGS += -nostdlib -ffreestanding -fvisibility=hidden
COM_FLAGS += -mno-red-zone -fno-asynchronous-unwind-tables
COM_FLAGS += -mmmx -msse -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2

# Kernel flags
KER_FLAGS = $(COM_FLAGS) -I src/kernel/include

# OS flags
OS_FLAGS = $(COM_FLAGS) -I src/os/include
OS_FLAGS += -I $(PORTS_DIR)/musl-1.2.2/include -I $(PORTS_DIR)/musl-1.2.2/obj/include
OS_FLAGS += -I $(PORTS_DIR)/musl-1.2.2/arch/x86_64 -I $(PORTS_DIR)/musl-1.2.2/arch/generic

KERNEL_LDFLAGS = -n -Map=map_kernel.txt -T linker_kernel.ld
OS_LDFLAGS = -n -Map=map_os.txt -T linker_os.ld
LDFLAGS = -n -Map=map.txt -T linker.ld

# Source files and corresponding object files
kernel_asm_sources = $(shell find src/kernel/assembly/src -name *.asm)
kernel_asm_objects = $(patsubst src/kernel/assembly/src/%.asm, build/kernel/assembly/%.o, $(kernel_asm_sources))

kernel_c_sources = $(shell find src/kernel/src -name *.c)
kernel_objects = $(patsubst src/kernel/src/%.c, build/kernel/%.o, $(kernel_c_sources))

os_asm_sources = $(shell find src/os/assembly/src -name *.asm)
os_asm_objects = $(patsubst src/os/assembly/src/%.asm, build/os/assembly/%.o, $(os_asm_sources))

os_c_sources = $(shell find src/os/src -name *.c)
os_objects = $(patsubst src/os/src/%.c, build/os/%.o, $(os_c_sources))
musl_object = $(PORTS_DIR)/musl-1.2.2/lib/libc.a

kernel_headers = $(shell find src/kernel/include -name *.h)
os_headers = $(shell find src/os/include -name *.h)

boot_includes = $(shell find src/boot/include -name *.asm)
bootloader_sources = $(shell find src/boot/src -name *.asm)
bootloader_objs = $(patsubst src/boot/src/%.asm, build/boot/%.o, $(bootloader_sources))

# Global constructor support
crti_obj = build/boot/crti.o
crtn_obj = build/boot/crtn.o
crtbegin_obj = $(shell $(CC) $(KER_FLAGS) -print-file-name=crtbegin.o)
crtend_obj = $(shell $(CC) $(KER_FLAGS) -print-file-name=crtend.o)

bootloader_objs := $(filter-out $(crti_obj) $(crtn_obj),$(bootloader_objs))

os_link_list = $(os_asm_objects) $(os_objects) $(musl_object)
os_object = build/os.o

kernel_link_list = $(crti_obj) $(crtbegin_obj) $(bootloader_objs) $(kernel_asm_objects) $(kernel_objects) $(crtend_obj) $(crtn_obj)
kernel_object = build/kernel.o

image_file = build/image.bin

# General build targets
build: $(kernel_binary) $(os_binary) count_sectors

ubsan_build: COM_FLAGS += -Os -fsanitize=undefined
ubsan_build: clean build

# OBJECT FILES - BOOT
$(bootloader_objs) $(crti_obj) $(crtn_obj) : build/boot/%.o : src/boot/src/%.asm $(boot_includes)
	mkdir -p $(dir $@) && \
	$(AS) -o $@ $<

# OBJECT FILES - KERNEL
$(kernel_asm_objects): build/kernel/assembly/%.o : src/kernel/assembly/src/%.asm
	mkdir -p $(dir $@) && \
	$(AS) $(patsubst build/kernel/assembly/%.o, src/kernel/assembly/src/%.asm, $@) -o $@

$(kernel_objects): build/kernel/%.o : src/kernel/src/%.c $(kernel_headers)
	mkdir -p $(dir $@) && \
	$(CC) -c -o $@ $< $(KER_FLAGS)

# OBJECT FILES - OS
$(os_asm_objects): build/os/assembly/%.o : src/os/assembly/src/%.asm
	mkdir -p $(dir $@) && \
	$(AS) $(patsubst build/os/assembly/%.o, src/os/assembly/src/%.asm, $@) -o $@

$(os_objects): build/os/%.o : src/os/src/%.c $(os_headers)
	mkdir -p $(dir $@) && \
	$(CC) -c -o $@ $< $(OS_FLAGS)

$(musl_object):
	( cd $(PORTS_DIR)/musl-1.2.2 && \
		CROSS_COMPILE=x86_64-elf- CC=x86_64-elf-gcc ./configure --target=x86_64 --disable-shared && \
		make -j )

# BINARIES - OS

$(kernel_object): $(kernel_link_list)
	$(LD) -r -o $@ $^ $(KERNEL_LDFLAGS) && \
	$(OBJCOPY) --localize-hidden $@

$(os_object): $(os_link_list)
	$(LD) -r -o $@ $^ $(OS_LDFLAGS) && \
	$(OBJCOPY) --localize-hidden $@

$(image_file): $(kernel_object) $(os_object)
	$(LD) -o $@ $^ $(LDFLAGS)

.PHONY: build ubsan qemu64 trace hvf clean count_sectors disassemble

QEMU_CMD = qemu-system-x86_64 -no-reboot \
           	-cpu qemu64,+mmx,+sse,+sse2,+sse3,+ssse3,+sse4a,+sse4.1,+sse4.2,+xsave \
           	-serial stdio \
           	-drive format=raw,file=$(image_file)

qemu64: build
	$(QEMU_CMD)

trace: build
	$(QEMU_CMD) -d in_asm 2>trace.txt

ubsan: ubsan_build qemu64

# cannot use SSE, so might as well compile with -Os and UBSAN
hvf: ubsan_build
	qemu-system-x86_64 -no-reboot \
	-M accel=hvf -cpu host,+xsave \
	-serial stdio \
	-drive format=raw,file=$(image_file)

count_sectors: $(image_file)
	@printf "\nSize of image.bin in sectors: "
	@echo "scale=4; `wc -c $(image_file) | sed 's/[^0-9]//g'` / 512" | bc -lq
	@printf "Currently loaded by bootloader: "
	@cat src/boot/include/sectors.asm | sed 's/[^0-9]//g'

disassemble:
	objdump -D -Mintel,x86-64 -b binary -m i386 build/image.bin > dis.txt

clean:
	rm -rf build/*
