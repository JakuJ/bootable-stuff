# Bootable stuff

**Work in progress.**

This repository contains a custom bootloader and an x86_64 kernel, both written from scratch.

![current state](docs/current.gif)

# Running

Make sure you have the required dependencies:

- make
- nasm
- QEMU (qemu-system-x86_64)
- x86_64-elf-g++
- x86_64-elf-ld

Then

```shell
make qemu64   # anywhere
make hvf      # only on MacOS
```

to build the image and boot it in QEMU.

# Features

- Bootloader
    - [x] Protected mode
    - [x] Long mode
    - [x] Enabling SSE, AVX*
- VGA controller
    - [x] Printing text to the screen
- Interrupts
    - [x] IDT setup with PIC remapping
    - [x] Event-based system for handing keyboard interrupts
    - [ ] Handling special keys and combinations (shift, backspace...)
    - [ ] Clock config (IRQ 0)
- Kernel
    - [ ] Handling page faults
    - [ ] Dynamic memory allocation (`malloc`, `new`)
- Standard library
    - [x] Basic `type_traits` API for template specialization
    - [x] Simple `std::function`-like wrappers
    - [x] VGA-independent `printf`
    - [ ] Porting an actual libc implementation

# Caveats

### AVX support

Provided that [XSAVE](https://wiki.osdev.org/SSE#XSAVE) is supported by the CPU, we can enable `AVX` within the kernel.
To do this, we enable the `avx` CPU feature in QEMU.

However, QEMU's [TCG](https://wiki.qemu.org/Features/TCG) cannot translate AVX instructions, and so they cannot be
emulated. This means that we cannot actually use AVX to vectorize kernel code. Attempting to compile the kernel
with `-mavx` will result in a **General Protection Fault**.

Running with MacOS's Hypervisor does not solve the issue.

### HVF acceleration

On MacOS, `qemu-system-x86_64` does not allow unsigned binaries to use Hypervisor acceleration.

To sign the QEMU binary, run the following command from repository root:

```shell
codesign -s - --entitlements qemu/app.entitlements --force "$(command -v qemu-system-x86_64)"
```
