section .text

extern GDT64_TSS
extern GDT64_Code
extern GDT64_Code_User
extern GDT64_Data_User

global flush_tss
flush_tss:
  mov ax, GDT64_TSS   ; tss selector
  or ax, 3            ; or with 3 to set the RPL (requested privilege level)
  ltr ax              ; load task register
  ret

global jump_usermode
jump_usermode:
  cli

  ; Enable the SYSCALL instruction in the EFER MSR
  mov ecx, 0xc0000080
  rdmsr
  or eax, 1
  wrmsr

  ; Setup STAR MSR
  ; Reference: https://www.sandpile.org/x86/msr.htm
  mov ecx, 0xc0000081
  mov edx, GDT64_Code_User
  or edx, 3
  shl edx, 16
  or edx, GDT64_Code
  mov eax, syscall_handler
  wrmsr

  ; Setup LSTAR and CSTAR MSRs
  mov ecx, 0xc0000082
  mov eax, syscall_handler
  mov rdx, syscall_handler
  shr rdx, 32
  wrmsr

  mov ecx, 0xc0000083
  wrmsr

  ; Setup SFMASK MSR
  mov ecx, 0xc0000084
  xor eax, eax
  xor edx, edx
  wrmsr

  ; Set segment registers to ring 3 data
  mov ax, GDT64_Data_User
  or ax, 3    ; RPL
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax  ; SS is handled by IRET

  ; Set up the stack frame IRET expects
  mov rax, rsp                    ; save current rsp

  push GDT64_Data_User            ; user data segment selector
  or qword [rsp], 3               ; RPL

  push rax                        ; current rsp
  mov [kernel_stack_save], rax

  pushfq                          ; RFLAGS
  or qword [rsp], 0x200           ; re-enable interrupts (set corresponding flag in RFLAGS)

  push GDT64_Code_User            ; user code segment selector
  or qword [rsp], 3               ; RPL

  extern os_entry
  push os_entry                   ; instruction address to return to
  iretq

syscall_handler:
  ; Restore kernel stack pointer
  mov [user_stack_save], rsp
  mov rsp, [kernel_stack_save]

  ; Preserve RIP stored by syscall in RCX
  push rcx

  ; Handle syscall in C
  extern handle_syscall
  call handle_syscall

  ; Restore RIP for sysret
  pop rcx

  ; Restore OS stack pointer
  mov [kernel_stack_save], rsp
  mov rsp, [user_stack_save]

  ; Return to ring 3
  o64 sysret ; the prefix forces 64-bit sysret (sysretq)

section .data

kernel_stack_save:  dq 0
user_stack_save:    dq 0
