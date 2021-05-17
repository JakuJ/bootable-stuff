ENTRY(boot)
OUTPUT_FORMAT("binary")
SECTIONS {
    . = 0x7c00;
    .boot : {
        _BOOT_START_ = .;
        *(.boot)
        *(.gdt)
        _BOOT_END_ = .;
    }
    kernel_text : ALIGN(4K)
    {
        _TEXT_START_ = .;
        build/kernel/*(.text)
        build/libc/*(.text)
        build/kernel/*(.rodata)
        build/libc/*(.rodata)
        _TEXT_END_ = .;
    }
    kernel_data : ALIGN(4K)
    {
        _DATA_START_ = .;
        build/boot/*(.data)
        build/kernel/*(.data)
        build/libc/*(.data)
        _DATA_END_ = .;
    }
    os_text : ALIGN(4K)
    {
        _OS_TEXT_START_ = .;
        build/os/*(.text)
        build/os/*(.rodata)
        _OS_TEXT_END_ = .;
    }
    os_data : ALIGN(4K)
    {
        _OS_DATA_START_ = .;
        build/os/*(.data)
        _OS_DATA_END_ = .;
    }
    kernel_bss : ALIGN(4K)
    {
        _BSS_START_ = .;
        build/boot/*(.bss)
        build/boot/*(COMMON)
        build/kernel/*(.bss)
        build/kernel/*(COMMON)
        _BSS_END_ = .;
    }
    os_bss : ALIGN(4K)
    {
        _OS_BSS_START_ = .;
        build/os/*(.bss)
        build/os/*(COMMON)
        _OS_BSS_END_ = .;
    }
    linker_first_free_page = ALIGN(4K);
}