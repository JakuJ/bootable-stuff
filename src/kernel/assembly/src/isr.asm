section .text

%include "src/kernel/assembly/include/push_all.asm"

%macro ISR_NOERRCODE 1
  global isr%1
  isr%1:
    push qword 0 ; Dummy error code for stack consistency
    push qword %1
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
  global isr%1
  isr%1:
    push qword %1
    jmp isr_common_stub
%endmacro

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE 8
ISR_NOERRCODE 9
ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20

isr_common_stub:
  cli                 ; disable interrupts on exception
  push_all            ; push edi,esi,ebp,esp,ebx,edx,ecx,eax

  xor rax, rax
  mov ax, ds
  push rax            ; save the data segment descriptor

  extern GDT64_Data
  mov rax, GDT64_Data ; load the kernel data segment descriptor
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  extern isr_handler  ; call kernel handler
  call isr_handler

  pop rax             ; reload the original data segment descriptor
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  pop_all             ; pop registers
  add rsp, 8          ; pop the pushed error code and pushed ISR number
  iretq               ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP
