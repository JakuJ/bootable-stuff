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

; Give control to the kernel
jmp 0x8000

; Fill the rest of the binary with zeros
times 510-($-$$) db 0

; Write MBR signature
dw 0xaa55
