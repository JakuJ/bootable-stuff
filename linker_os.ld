ENTRY(os_entry)
OUTPUT_FORMAT("elf64-x86-64")
SECTIONS {
    os_text :
    {
        _OS_TEXT_START_ = .;
        *(.text*)
        _OS_TEXT_END_ = .;
    }
    os_rodata : ALIGN(4K)
    {
        _OS_RODATA_START_ = .;
        *(.rodata*)
        *(.tm_clone_table)
        _OS_RODATA_END_ = .;
    }
    os_data : ALIGN(4K)
    {
        _OS_DATA_START_ = .;
        *crt*.o(.data*)
        *(.data*)
        _OS_DATA_END_ = .;
    }
    os_bss : ALIGN(4K)
    {
        _OS_BSS_START_ = .;
        *(.bss*)
        *crt*.o(.bss*)
        _OS_BSS_END_ = .;
    }
    /DISCARD/ :
    {
      *(.comment*)
      *(.eh_frame*)
      *(.note*)
    }
}