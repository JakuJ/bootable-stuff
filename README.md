# Bootable stuff

**Work in progress.**

This repository contains a custom bootloader and an x86_64 kernel written, both written from scratch.

![current state](docs/current.gif)

# Running

Make sure you have the required dependencies:

- nasm
- QEMU (qemu-system-x86_64)
- i386-elf-g++
- i386-elf-ld
- x86_64-elf-g++
- x86_64-elf-ld

Then

```
make qemu64
```

to build the image and boot it in QEMU.

# Features

- Bootloader
    - [x] Protected mode
    - [x] Long mode
    - [x] SSE, AVX (see: notes)
    - [ ] Compiling for both 32-bit and 64-bit?
- VGA controller
    - [x] Printing text to the screen
    - [ ] An actual graphics API
- Interrupts
    - [x] IDT setup with PIC remapping
    - [x] Event-based system for handing keyboard interrupts
    - [ ] Handling special keys and combinations (shift, backspace...)
- Custom standard library for C++
    - [x] Basic `type_traits` API for template specialization
    - [x] Simple `std::function`-like wrappers
    - [ ] Dynamic memory allocation (`malloc`?, `new` operators)
    - [ ] VGA-independent `printf`

# Caveats

### AVX support

Provided that [XSAVE](https://wiki.osdev.org/SSE#XSAVE) is supported by the CPU, we can enable `AVX` within the kernel.
To do this, we enable the `avx` CPU feature in QEMU.

However, QEMU's [TCG](https://wiki.qemu.org/Features/TCG) cannot translate AVX instructions, and so they cannot be
emulated. This means that we cannot actually use AVX to vectorize kernel code. Attempting to compile the kernel
with `-mavx` will result in a **General Protection Fault**.
