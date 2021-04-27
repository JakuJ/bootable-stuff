bits 16    ; use 16 bits
org 0x7c00 ; set the starting address

; Reset disk system
mov ah, 0
int 0x13 ; dl = drive number (already set by the BIOS)

; Read from hard drive and write to RAM
mov bx, 0x8000  ; bx = address to write the kernel to
mov al, 1 		  ; al = number of sectors to read
mov ch, 0       ; cylinder/track = 0
mov dh, 0       ; head           = 0
mov cl, 2       ; sector         = 2
mov ah, 2       ; ah = 2: read from drive
int 0x13   		  ; => ah = status, al = amount read

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

; Transfer control to the kernel
jmp KERNEL_SEG:kernel

bits 32
kernel:
  ; Set the remaining segments to point at the data segment
  mov ax, DATA_SEG
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  ; Print something to see if kernel loads in 32 bit
  mov esi, hello
  mov ebx, 0xb8000 ; VGA text buffer location
  .loop:
    lodsb
    or al, al
    jz halt
    or eax, 0x0f00      ; BG color (4 bits), FG color (4 bits), ASCII character (byte)
    mov word [ebx], ax  ; "print" by writing to VGA buffer
    add ebx, 2          ; move to next space in buffer
    jmp .loop
  halt:
    cli
    hlt

hello: db "Hello from 32 bits!", 0x0a, 0

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
KERNEL_SEG equ gdt_kernel - gdt_start
DATA_SEG equ gdt_data - gdt_start

; Fill the rest of the binary with zeros
times 510-($-$$) db 0

; Write MBR signature
dw 0xaa55
