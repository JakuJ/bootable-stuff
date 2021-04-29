# Bootable stuff

**Work in progress.**

This repository contains an x86 bootloader and kernel written from scratch.

The main goal is to learn x86 assembly and OS/kernel programming.

![current state](docs/current.gif)

## Running

Install these dependencies:

- nasm
- qemu-system-x86_64
- i386-elf-gcc

Then

```
make run
```

to build the image and boot it in QEMU.

## Features

- Bootloader
    - [x] Protected mode
    - [ ] Long mode?
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
