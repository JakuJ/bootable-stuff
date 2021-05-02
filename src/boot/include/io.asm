; FUNCTION
; Print a character to console.
; Inputs:
; al - input character
putChar:
  push bx
  mov bh, 0x00 ; page to write to, page 0 is displayed by default
  mov bl, 0x1f ; color attribute, white on blue
  mov ah, 0x0e ; function 0e = print character in al
  int 10h
  pop bx
  ret
; END

; FUNCTION
; Print a 0-terminated string to console.
; Inputs:
; si = input string
putStr:
  push ax         ; save register state
  .loop:
    lodsb         ; load byte from si into al and increases si
    cmp al, 0     ; test for NULL-terminator
    jz .end
    call putChar
  jmp .loop       ; print next character
  .end:
    pop ax        ; restore register to original state
    ret
; END

; FUNCTION
; Reads a single character from keyboard.
; Outputs:
; al = character read (ASCII)
; ah = character read (keycode)
; zf = 1 if nothing read
waitForKeyPress:
  mov ah, 1
  int 0x16            ; int 0x16, 1 => check if a key is pressed (set zero flag if not)
  jz waitForKeyPress
  mov ah, 0           ; int 0x16, 0 => halts till key is pressed and returns it into al and ah
  int 0x16
  ret
; END
