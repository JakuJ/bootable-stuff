; FUNCTION
; Print a character to console.
; Inputs:
; al - input character
putChar:
  push bx
  mov bh, 0x00 ; page to write to, page 0 is displayed by default
  mov bl, 0x00 ; color attribute, doesn't matter for now
  mov ah, 0x0E ; function 0E = print character in al
  int 0x10
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
; Prints \r\n to console.
printLn:
  push ax
  mov al, 13d
  call putChar
  mov al, 10d
  call putChar
  pop ax
  ret
; END

; FUNCTION
; Clear the terminal screen.
clearScreen:
  push ax
  mov al, 2 ; Set video mode to 80x25
  mov ah, 0
  int 0x10  ; int 0x10, 0 => change VGA mode
  pop ax
  ret
; END

; FUNCTION
; Reads a single character from keyboard.
; Outputs:
; al = character read (ASCII)
; ah = character read (keycode)
; zf = 1 if nothing read
readChar:
  mov ah, 1
  int 0x16  ; int 0x16, 1 => check if a key is pressed (set zero flag if not)
  jz .end
  mov ah, 0 ; int 0x16, 0 => halts till key is pressed and returns it into al and ah
  int 0x16
  ret
  .end:
  mov ax, 0 ; if no character was pressed return al = 0 and ah = 0
  ret
; END

; FUNCTION
; Reads a string from keyboard.
; Outputs:
; di = the string
; zf = 1 if nothing read
; Changes ax.
%define readString_size 8 ; max number of characters
readString:
  mov di, readString_buffer            ; stosb writes into di so we set di to the start of the buffer
  .inner:
    call readChar                      ; read a character
    jz .inner                          ; if nothing read, repeat reading until enter pressed or max size reached
    cmp ah, 0x1C                       ; check if ENTER pressed
    je .end
    cmp ah, 0x0E                       ; check if BACKSPACE pressed
    je .remove
    stosb                              ; store character into buffer and increase di
    ; print the character (live feedback)
    pusha
    call putChar
    popa
    ; if length of di is >= readString_size, go to end
    cmp di, (readString_buffer+readString_size)
    jge .end
  jmp .inner                    ; read next character
  .remove:
    cmp di, readString_buffer   ; if di is at index 0 do not remove character as it is already at the beginning
    jle .inner
    dec di                      ; remove a character by moving one index back
    ; go one character back, empty it with space and then go back again
    pusha
    call putChar
    mov al, ' '
    call putChar
    mov al, 8
    call putChar
    popa
    jmp .inner
  .end:
    ; zero-terminate string
    xor al, al
    stosb
    ; print newline on input end
    pusha
    call printLn
    popa
    ; set output to buffer beginning
    mov di, readString_buffer
    ret
; END

; FUNCTION
; Compare whether two strings are equal.
; Inputs:
; di = first string
; si = second string
; Outputs:
; zf = 1 if equal
stringCompare:
  pusha
  or cx, -1   ; set cx to biggest unsigned number
  xor al, al  ; clear al
  repne scasb ; scan through di until zero terminator was hit and decrease cx for each scanned character
  neg cx      ; calculate length of di by negating cx which returns the length of the string including zero terminator
  sub di, cx  ; reset di by setting it to the original index it started with
  inc di
  repe cmpsb  ; check if character from di match with si (including zero terminator)
  test cx, cx ; test if number of matching = size of string, set zero flag if equals
  popa
  ret
; END
