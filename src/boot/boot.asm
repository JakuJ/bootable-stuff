section .boot
bits 16

; Define global symbol for the linker
global boot
boot:

; Print welcome message
mov si, welcome
call putStr

; Wait for user input
call waitForKeyPress

; Reset disk system
mov ah, 0
int 0x13 ; dl = drive number (already set by the BIOS)

; Read from hard drive and write to RAM
mov bx, kernel_copy_target  ; bx = address to write the kernel to
mov al, [sectors_to_read] 	; al = number of sectors to read
mov ch, 0       ; cylinder/track = 0
mov dh, 0       ; head           = 0
mov cl, 2       ; sector         = 2
mov ah, 2       ; ah = 2: read from drive
int 0x13   		  ; Sets: ah = status, al = amount read

; ENTER 32-bit PROTECTED MODE

; Disable interrupts
cli

; Enable the A20 line
in al, 0x92
or al, 2
out 0x92, al

; Load GDT
lgdt [GDT32.Pointer]

; Set PE (Protection Enable) bit in CR0 (Control Register 0)
mov eax, cr0
or al, 1
mov cr0, eax

; Set the remaining segments to point at the data segment
mov ax, GDT32.Data
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax

; Enter 32-bit protected mode
jmp GDT32.Code:enable_long_mode

bits 32

enable_long_mode:
  call check_cpuid
  call check_long_mode
  call setup_paging
  call enable_sse

  lgdt [GDT64.Pointer]                    ; Load the 64-bit GDT
  jmp GDT64.Code:kernel_copy_target       ; Enter 64-bit long mode.

%include "src/boot/include/long_mode.asm"

bits 16

; Include other functions
%include "src/boot/include/io.asm"
%include "src/boot/include/sectors.asm"

; Include Global Descriptor Tables for 32-bit and 64-bit modes
%include "src/boot/include/gdt.asm"

; Welcome message data
welcome db 10d, "Bootloader v1.0", 13d, 10d, "Press any key...", 0

; Fill the rest of the bootloader binary with zeros
times 510 - ($ - $$) db 0

; Write MBR signature
dw 0xaa55

; Kernel code will be copied here
kernel_copy_target:
bits 64

; Set segment registers to point at the data segment
mov ax, GDT64.Data
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax

; Update stack pointer
mov rsp, kernel_stack_top

; Execute global constructors
extern _init
call _init

; Execute kernel code
extern kmain
call kmain

; Execute cleanup routines
extern _fini
call _fini

; Halt
cli
hlt

section .bss
align 4

; Allocate 16KB of stack space in 0-filled section
kernel_stack_bottom: equ $
resb 16384
kernel_stack_top: