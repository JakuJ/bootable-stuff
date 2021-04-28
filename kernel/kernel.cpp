// Kernel entry point
extern "C" void kmain() {
    const short color = 0x1f00; // white on blue
    const char hello[] = "Hello world from the kernel!";
    short *vga = (short *) 0xb8000;

    for (int i = 0; i < (int) sizeof(hello); ++i) {
        vga[i + 10 * 80] = color | hello[i]; // write to 10th row
    }
}
