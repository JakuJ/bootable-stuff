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
    .text : ALIGN(4K)
    {
        _TEXT_START_ = .;
        *(.text)
        _TEXT_END_ = .;
    }
    .rodata : ALIGN(4K)
    {
        _RODATA_START_ = .;
        *(.rodata)
        _RODATA_END_ = .;
    }
    .data : ALIGN(4K)
    {
        _DATA_START_ = .;
        *(.data)
        _DATA_END_ = .;
    }
    .bss : ALIGN(4K)
    {
        _BSS_START_ = .;
        *(.bss)
        *(COMMON)
        _BSS_END_ = .;
    }
}