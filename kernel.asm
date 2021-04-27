bits 16    ; use 16 bits
org 0x8000 ; set the starting address

section .text

call clearScreen

start:
  ; Print message
  mov si, msg
  call putStr

  ; Read input
  call readString

  ; Parse input
  mov si, di
  call decstr2num
  jz halt

  ; Print doubled
  mov ax, dx
  imul ax, 2
  call printDecimal
  call printLn
jmp start

halt:
  ; Announce halting
  mov si, halted
  call putStr
  call printLn
  hlt ; halt the system

%include "src/convert.asm"

section .bss

readString_buffer: resb (readString_size+1)

section .rodata

msg db "Enter number to be doubled: ", 0
halted db "Halted!", 0

; Padding
times 512-($-$$) db 0 ; kernel must have size multiple of 512 so let's pad it to the correct size