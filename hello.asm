bits 16    ; use 16 bits
org 0x7c00 ; set the starting address

clearScreen:
  mov ax, 0x0700  ; function 07, AL=0 means scroll whole window
  mov bh, 0x07    ; character attribute = white on black
  mov cx, 0x0000  ; row = 0, col = 0
  mov dx, 0x184f  ; row = 24 (0x18), col = 79 (0x4f)
  int 0x10        ; call BIOS video interrupt

setCursorTop:
  mov ah, 0x02    ; function 02, set cursor position
  mov bh, 0       ; page
  mov dx, 0x0000  ; DH: row, DL: column (0, 0)
  int 0x10

start:
  mov si, msg  ; load the address of "message" into the SI register
  mov ah, 0x0e ; set higher bits to the display character command (function teletype)

.loop:
  lodsb     ; load the byte within the AL register, and increment SI
  cmp al, 0
  je halt   ; if AL == 0 (reached null terminator), finish looping
  int 0x10  ; print to screen using function 0xe of interrupt 0x10 (video services)
  jmp .loop

halt:
  hlt

msg: db "Hello world!", 0x0d, 0x0a, 0 ; \r, \n, null terminator

; mark the device as bootable
times 510-($-$$) db 0           ; fill the output file with zeroes until 510 bytes are full
dw 0xaa55                       ; write the final 2 bytes as the magic number 0x55aa, remembering x86 little endian