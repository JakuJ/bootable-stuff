section .text

%macro ISR_NOERRCODE 1
  global isr%1
  isr%1:
    cli
    push byte 0
    push byte %1
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
  global isr%1
  isr%1:
    cli
    push byte %1
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
ISR_NOERRCODE 17
ISR_NOERRCODE 18

isr_common_stub:
  pusha                    ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

  mov ax, ds               ; Lower 16-bits of eax = ds.
  push eax                 ; save the data segment descriptor

  extern DATA_SEG
  mov ax, DATA_SEG         ; load the kernel data segment descriptor
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  extern isr_handler       ; call kernel handler
  call isr_handler

  pop eax                  ; reload the original data segment descriptor
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  popa                     ; Pops edi,esi,ebp...
  add esp, 8               ; Cleans up the pushed error code and pushed ISR number
  sti
  iret                     ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP
