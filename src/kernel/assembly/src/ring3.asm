section .text

%include "src/kernel/assembly/include/push_all.asm"

extern GDT64_TSS
extern GDT64_Code
extern GDT64_Code_User
extern GDT64_Data_User

global flush_tss
flush_tss:
  mov ax, GDT64_TSS   ; tss selector
  ltr ax              ; load task register
  ret

global jump_usermode
jump_usermode:
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
  mov eax, 0x200                ; mask interrupts
  xor edx, edx
  wrmsr

  ; Set segment registers for ring 3
  mov ax, GDT64_Data_User
  or ax, 3
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax                    ; SS is handled by IRET

  ; Jump to ring 3
  pushf                         ; leave EFLAGS unchanged
  pop r11

  mov [kernel_stack_save], rsp  ; load the user stack pointer
  mov rsp, [user_stack_save]

  extern os_entry
  mov rcx, os_entry             ; set the RIP after sysret to OS entry point

  o64 sysret                    ; the prefix forces 64-bit sysret (sysretq)

syscall_handler:
  ; Restore kernel stack pointer
  mov [user_stack_save], rsp
  mov rsp, [kernel_stack_save]

  ; Preserve EFLAGS stored in R11
  push r11

  ; Preserve RIP stored in RCX
  push rcx

  ; Save C ABI callee-preserved registers
  push_all

  ; musl's syscall places arg 4 in r10,
  ; so we move it to RCX for SYSV ABI compliance
  mov rcx, r10

  ; Same with arg. 6 - move from RAX to stack
  push rax

  ; Handle syscall in C
  extern handle_syscall
  call handle_syscall

  ; Save return value in RAX for later
  mov [syscall_retval], rax

  ; Pop that 6th argument from the stack
  add rsp, 8

  ; Restore C ABI callee-preserved registers
  pop_all

  ; Restore RIP for sysret
  pop rcx

  ; sysret will restore EFLAGS from r11
  pop r11 ; TODO: interrupts in ring 3 cause #GP pointing to the TSS

  ; Restore OS stack pointer
  mov [kernel_stack_save], rsp
  mov rsp, [user_stack_save]

  ; Remember to place that syscall return value in RAX
  mov rax, [syscall_retval]

  ; Return to ring 3
  o64 sysret

section .data

kernel_stack_save:  dq 0
user_stack_save:    dq 0
syscall_retval:     dq 0
