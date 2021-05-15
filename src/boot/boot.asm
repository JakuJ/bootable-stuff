section .boot
bits 16

; Define global symbol for the linker
global boot
boot:

; Reset disk system
mov ah, 0
int 0x13 ; dl = drive number (already set by the BIOS)

; Read from hard drive and write to RAM
mov bx, stage2  ; bx = address to write the kernel to
mov al, [sectors_to_read] 	; al = number of sectors to read
mov ch, 0       ; cylinder/track = 0
mov dh, 0       ; head           = 0
mov cl, 2       ; sector         = 2
mov ah, 2       ; ah = 2: read from drive
int 0x13   		  ; Sets: ah = status, al = amount read

jmp stage2

; Bootloader data section
%include "src/boot/include/sectors.asm"

; Fill the rest of the bootloader binary with zeros
times 510 - ($ - $$) db 0

; Write MBR signature
dw 0xaa55

; Stage 2 will be loaded here
stage2:

%include "src/boot/include/real_mode.asm"
%include "src/boot/include/protected_mode.asm"
%include "src/boot/include/long_mode.asm"
