bits 64

long_mode:

; Set segment registers to point at the data segment
mov ax, GDT64_Data
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

; Allocate 16KB of stack space
kernel_stack_bottom: equ $
resb 16384
kernel_stack_top: