; Set VGA mode
mov ah, 0
mov al, 0x13
int 0x10

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

; Set PE (Protection Enable) bit in CR0 (Control Register 0)
mov eax, cr0
or al, 1
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