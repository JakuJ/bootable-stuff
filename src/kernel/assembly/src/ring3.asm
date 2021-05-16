section .text

extern GDT64_TSS
extern GDT64_Pointer
extern GDT64_Code

global flush_tss
flush_tss:
  mov ax, GDT64_TSS ; tss selector
  or ax, 3          ; or with 3 to set the RPL (requested privilege level).
  ltr ax
  ret

global jump_usermode
;extern test_user_function
jump_usermode:
  cli

  xor rax, rax
  mov ax, (4 * 8) | 3 ; ring 3 data with bottom 2 bits set for ring 3
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax ; SS is handled by iret

  ; Set up the stack frame iret expects
  mov rax, rsp
  push (4 * 8) | 3                ; user data segment selector
  push rax                        ; current rsp
  pushfq                          ; eflags
  or qword [rsp], 0x200           ; re-enable interrupts (set corresponding flag in EFLAGS)
  push (3 * 8) | 3                ; user code segment selector
  push test_user_function         ; instruction address to return to
  iretq

test_user_function:
  ret