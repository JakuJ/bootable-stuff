section .boot
bits 16

; Define global symbol for the linker
global boot
boot:

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

; DISPLAY SPLASH SCREEN
call clearScreen

; Move cursor to the top
mov ah, 2
mov bh, 0
mov dx, 0
int 10h

; Print welcome message
mov si, title
call putStr

; Move cursor to the second line
mov ah, 2
mov bh, 0
mov dh, 1
int 10h

; Print the other message
;mov si, msg
;call putStr

; Wait for user input
;call readString

; ENTER 32-bit PROTECTED MODE

; Disable interrupts
cli

; Enable the A20 line
in al, 0x92
or al, 2
out 0x92, al

; Load GDT
lgdt [gdt_pointer]

; Set PE (Protection Enable) bit in CR0 (Control Register 0)
mov eax, cr0
or al, 1
mov cr0, eax

; Set the remaining segments to point at the data segment
mov ax, DATA_SEG
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax

; Enable long mode
jmp KERNEL_SEG:long_mode

bits 32

; Enabling long mode
long_mode:
  call check_cpuid
  call check_long_mode
  call setup_paging

  lgdt [GDT64.Pointer]                    ; Load the 64-bit global descriptor table.
  jmp GDT64.Code:kernel_copy_target       ; Set the code segment and enter 64-bit long mode.

%include "src/boot/include/long_mode.asm"

bits 16

; Include other functions
%include "src/boot/include/io.asm"
%include "src/boot/include/sectors.asm"

; Define GDT
gdt_start:
  ; null segment
  dq 0x0
gdt_kernel:
  ; kernel code segment
  dw 0xffff        ; bits 0:15  - limit 0:15, set to 4GB
  dw 0x0           ; bits 16:31 - base 0:15, set to 0
  db 0x0           ; bits 32:39 - base 16:23
  db 0b10011010    ; bits 40:47 - access byte
  db 0b11001111    ; bits 48:55 - limit 16:29 + flags
  db 0x0           ; bits 56:63 - base 24:31
gdt_data:
  ; data segment
  dw 0xffff        ; bits 0:15  - limit 0:15, set to 4GB
  dw 0x0           ; bits 16:31 - base 0:15, set to 0
  db 0x0           ; bits 32:39 - base 16:23
  db 0b10010010    ; bits 40:47 - access byte
  db 0b11001111    ; bits 48:55 - limit 16:29 + flags
  db 0x0           ; bits 56:63 - base 24:31
gdt_end:

; Calculate GDT pointer for lgdt
gdt_pointer:
    dw gdt_end - gdt_start  ; size
    dd gdt_start            ; offset

; Calculate segments (offsets into GDT e.g. for long jumps)
global DATA_SEG
KERNEL_SEG equ gdt_kernel - gdt_start
DATA_SEG equ gdt_data - gdt_start

GDT64:                           ; Global Descriptor Table (64-bit).
  .Null: equ $ - GDT64         ; The null descriptor.
  dw 0xFFFF                    ; Limit (low).
  dw 0                         ; Base (low).
  db 0                         ; Base (middle)
  db 0                         ; Access.
  db 1                         ; Granularity.
  db 0                         ; Base (high).
  .Code: equ $ - GDT64         ; The code descriptor.
  dw 0                         ; Limit (low).
  dw 0                         ; Base (low).
  db 0                         ; Base (middle)
  db 10011010b                 ; Access (exec/read).
  db 10101111b                 ; Granularity, 64 bits flag, limit19:16.
  db 0                         ; Base (high).
  .Data: equ $ - GDT64         ; The data descriptor.
  dw 0                         ; Limit (low).
  dw 0                         ; Base (low).
  db 0                         ; Base (middle)
  db 10010010b                 ; Access (read/write).
  db 00000000b                 ; Granularity.
  db 0                         ; Base (high).
  .Pointer:                    ; The GDT-pointer.
  dw $ - GDT64 - 1             ; Limit.
  dq GDT64                     ; Base.

; Strings
title db "Bootloader v1.0", 0
;msg db "Press ENTER to load the kernel...", 0

; Fill the rest of the bootloader binary with zeros
times 510-($-$$) db 0

; Write MBR signature
dw 0xaa55

bits 64

; Kernel code will be copied here
kernel_copy_target:
cli                           ; Clear the interrupt flag.
mov ax, GDT64.Data            ; Set the A-register to the data descriptor.
mov ds, ax                    ; Set the data segment to the A-register.
mov es, ax                    ; Set the extra segment to the A-register.
mov fs, ax                    ; Set the F-segment to the A-register.
mov gs, ax                    ; Set the G-segment to the A-register.
mov ss, ax                    ; Set the stack segment to the A-register.

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

; Buffer for IO operations
readString_buffer: resb (readString_size+1)

; Allocate 16KB of stack space in 0-filled section
kernel_stack_bottom: equ $
resb 16384 ; 16 KB
kernel_stack_top: