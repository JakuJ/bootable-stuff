; Set VBE mode
mov ax, 800d
mov bx, 600d
mov cl, 32d
call vbe_set_mode

; Disable interrupts
cli

; Enable the A20 line
in al, 0x92
or al, 2
out 0x92, al

; Load the 32-bit GDT
extern GDT32_Code
extern GDT32_Data
extern GDT32_Pointer
extern GDT64_Code
extern GDT64_Data
extern GDT64_Pointer

lgdt [GDT32_Pointer]

; Set bits in CR0 (Control Register 0)
mov eax, cr0
or eax, 1        ; set PE (Protection Enable) bit
or eax, 1 << 16  ; set WP (Write protect) bit
mov cr0, eax

; Set the remaining segments to point at the data segment
mov ax, GDT32_Data
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax

; Enter 32-bit protected mode
jmp GDT32_Code:protected_mode

%include "src/boot/include/vesa.asm"
