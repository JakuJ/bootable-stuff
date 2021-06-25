ENTRY(boot)
OUTPUT_FORMAT("binary")
SECTIONS {
    . = 0x7c00;
    .boot : {
        build/kernel.o(.boot)
    }
    kernel_text : ALIGN(4K)
    {
        build/kernel.o(kernel_text)
    }
    kernel_rodata : ALIGN(4K)
    {
        build/kernel.o(kernel_rodata)
    }
    kernel_data : ALIGN(4K)
    {
        build/kernel.o(kernel_data)
    }
    os_text : ALIGN(4K)
    {
        build/os.o(os_text)
    }
    os_rodata : ALIGN(4K)
    {
        build/os.o(os_rodata)
    }
    os_data : ALIGN(4K)
    {
        build/os.o(os_data)
    }
    kernel_bss : ALIGN(4K)
    {
        build/kernel.o(kernel_bss)
    }
    os_bss : ALIGN(4K)
    {
        build/os.o(os_bss)
    }
    _FIRST_FREE_PAGE_ = ALIGN(4K);
}