ENTRY(boot)
OUTPUT_FORMAT("elf64-x86-64")
SECTIONS {
    .boot : {
        _BOOT_START_ = .;
        *(.boot)
        *(.gdt)
        _BOOT_END_ = .;
    }
    kernel_text : ALIGN(4K)
    {
        _KERNEL_TEXT_START_ = .;
        *(.init)
        *(.fini)
        *(.ctors)
        *(.dtors)
        *crt*.o(.text*)
        build/kernel/*(.text*)
        _KERNEL_TEXT_END_ = .;
    }
    kernel_rodata : ALIGN(4K)
    {
        _KERNEL_RODATA_START_ = .;
        build/kernel/*(.rodata*)
        *crt*.o(.rodata*)
        *(.tm_clone_table)
        *(.eh_frame)
        _KERNEL_RODATA_END_ = .;
    }
    kernel_data : ALIGN(4K)
    {
        _KERNEL_DATA_START_ = .;
        build/boot/*(.data)
        build/kernel/*(.data)
        *crt*.o(.data)
        _KERNEL_DATA_END_ = .;
    }
    kernel_bss : ALIGN(4K)
    {
        _KERNEL_BSS_START_ = .;
        build/boot/*(.bss)
        *crt*.o(.bss)
        build/kernel/*(.bss)
        _KERNEL_BSS_END_ = .;
    }
    /DISCARD/ :
    {
      *(.comment*)
      *(.note*)
    }
}