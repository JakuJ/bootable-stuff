section .text

global os_entry
os_entry:
  mov rsp, user_stack_top

  extern os_main
  call os_main

  jmp $

section .bss

align 4
user_stack_bottom:
resb 16384
user_stack_top:
