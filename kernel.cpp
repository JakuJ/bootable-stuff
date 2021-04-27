// Kernel entry point
extern "C" void kmain() {
    const short color = 0x0F00; // white
    const char hello[] = "Hello world from C++!";
    short *vga = (short *) 0xb8000;

    for (int i = 0; i < (int) sizeof(hello); ++i) {
        vga[i + 10 * 80] = color | hello[i]; // write to 10th row
    }
}
