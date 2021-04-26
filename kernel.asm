bits 16    ; use 16 bits
org 0x8000 ; set the starting address

call clearScreen
mov si, msg
call printNullTerminatedString
hlt ; halt the system
ret ; halt QEMU

; Functions
putChar:
  ; al must be set to the character to print
  mov bh, 0x00 ; page to write to, page 0 is displayed by default
  mov bl, 0x00 ; color attribute, doesn't matter for now
  mov ah, 0x0E ; function 0E = print character in al
  int 0x10
  ret

printNullTerminatedString:
	pusha           ; save register state
	.loop:
		lodsb         ; load byte from si into al and increases si
		cmp al, 0     ; test for NULL-terminator
		jz .end
		call putChar
	jmp .loop       ; print next character
	.end:
	  popa          ; restore registers to original state
	  ret

clearScreen:
  pusha
  mov ax, 0x0700  ; function 07 = scroll window down, AL=0 means scroll whole window
  mov bh, 0x07    ; character attribute = white on black
  mov cx, 0x0000  ; row = 0, col = 0 - upper left corner coords
  mov dx, 0x184f  ; row = 24 (0x18), col = 79 (0x4f) - lower right corner coords
  int 0x10

  mov bh, 0 ; page to write to
  mov dx, 0 ; DH: row, DL: column (0, 0)
  mov ah, 2 ; function 02 = set cursor position
  int 0x10
  popa
  ret

; Data
msg db "Hello World!", 0x0d, 0x0a, 0 ; \r, \n, NULL

; Padding
times 512-($-$$) db 0 ; kernel must have size multiple of 512 so let's pad it to the correct size