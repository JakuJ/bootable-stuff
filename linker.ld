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
        *(.init)
        *(.fini)
        *(.ctors)
        *(.dtors)
        *crt*.o(.text*)
        build/kernel/*(.text*)
        _TEXT_END_ = .;
    }
    kernel_rodata : ALIGN(4K)
    {
        _RODATA_START_ = .;
        build/kernel/*(.rodata*)
        *crt*.o(.rodata*)
        *(.tm_clone_table)
        _RODATA_END_ = .;
    }
    kernel_data : ALIGN(4K)
    {
        _DATA_START_ = .;
        build/boot/*(.data)
        build/kernel/*(.data)
        *crt*.o(.data)
        _DATA_END_ = .;
    }
    os_text : ALIGN(4K)
    {
        _OS_TEXT_START_ = .;
        build/os/*(.text*)
        external/musl-1.2.2/lib/libc.a(.text*)
        _OS_TEXT_END_ = .;
    }
    os_rodata : ALIGN(4K)
    {
        _OS_RODATA_START_ = .;
        build/os/*(.rodata*)
        external/musl-1.2.2/lib/libc.a(.rodata*)
        _OS_RODATA_END_ = .;
    }
    os_data : ALIGN(4K)
    {
        _OS_DATA_START_ = .;
        build/os/*(.data)
        external/musl-1.2.2/lib/libc.a(.data*)
        _OS_DATA_END_ = .;
    }
    kernel_bss : ALIGN(4K)
    {
        _BSS_START_ = .;
        build/boot/*(.bss)
        *crt*.o(.bss)
        build/kernel/*(.bss)
        _BSS_END_ = .;
    }
    os_bss : ALIGN(4K)
    {
        _OS_BSS_START_ = .;
        build/os/*(.bss)
        external/musl-1.2.2/lib/libc.a(.bss*)
        _OS_BSS_END_ = .;
    }
    /DISCARD/ :
    {
      *(.comment*)
      *(.eh_frame*)
      *(.note*)
    }
    _FIRST_FREE_PAGE_ = ALIGN(4K);
}