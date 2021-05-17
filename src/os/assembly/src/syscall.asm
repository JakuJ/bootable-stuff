section .text

global $syscall
$syscall:
  push r11    ; r11 is always destroyed by syscall
  syscall
  pop r11
  ret
