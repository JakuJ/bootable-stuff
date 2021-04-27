; FUNCTION
; Convert decimal string to number.
; Inputs:
; si = input string
; Outputs:
; dx = number
; zf = 1 if failed
decstr2num:
  push ax         ; save ax and si
  xor dx, dx      ; reset dx and ax to use them for work
  xor ax, ax
  .loop:
    lodsb         ; load ASCII character from input string SI into AL and increase SI
    test al, al   ; end reading if reached zero terminator
    jz .end
    imul dx, 0xa  ; multiply by 10
    cmp al, '0'
      jl .error   ; if character is less than '0' it can not be a number
    cmp al, '9'
      jle .num    ; if character is within the range of '0' - '9' it is a number
    jmp .error    ; if it is not a number or a hex character => error
    .num:
      sub al, '0' ; subtract 0x30 from ASCII number to get value
      add dx, ax  ; lastResult = (lastResult * 10) + currentNumber;
    jmp .loop	    ; loop to the next character
  .end:
  xor ax, ax
  cmp ax, 1       ; ax != 1 - zero flag not set
  jmp .finish
  .error:
    xor dx, dx    ; set output to 0
    test dx, dx   ; set zero flag to indicate error
  .finish:
  pop ax
  ret
; END

; FUNCTION
; Prints a number to console.
; Inputs:
; ax = input number
printDecimal:
  push cx
  push dx

  mov cx, 10d
  xor dx, dx
  ; to print a 16-bit register, we need at most 5 digits
  ; div divides dx:ax by 10 (cx) into ax (quotient) and dx (remainder)
  %rep 5
  div cx
  push dx
  xor dx, dx
  %endrep

  mov bx, 0 ; 0 - leading zeros, 1 - no longer leading zeros
  mov cx, 5 ; counter
  .loop:
    ; loop counter management
    jcxz .end
    dec cx
    ; pop nex digit and check if zero
    pop ax
    cmp ax, 0
    or bx, ax ; if ax is not zero, bx won't be either
    cmp bx, 0
    je .loop ; if leading zero, skip it
    add al, '0'
    call putChar
  jmp .loop
  .end:
    pop dx
    pop cx
    ret
; END