bits 32
org 0x7e00            ; bootloader offset (0x7c00) + 1 sector

mov esi, hello
mov ebx, 0xb8000      ; VGA buffer address
.loop:
  lodsb
  or al, al           ; check for \0
  jz halt
  or eax, 0x0f00      ; VGA payload: BG color (4 bits), FG color (4 bits), ASCII char (1 byte)
  mov word [ebx], ax  ; print to screen by writing directly to VGA buffer
  add ebx, 2          ; move to next space in the buffer
jmp .loop
halt:
  cli
  hlt

hello: db "Hello world from another sector!", 0x0a, 0

; Padding
times 1024-($-$$) db 0 ; kernel must have size multiple of 512