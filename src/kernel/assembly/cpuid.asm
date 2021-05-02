section .code

global query_cpu:
query_cpu:
  push rbx
  push rdx

  mov eax, edi  ; page
  mov ecx, 0
  cpuid

  cmp esi, 0    ; register
  je .read_eax
  cmp esi, 1
  je .read_ebx
  cmp esi, 2
  je .read_ecx
  cmp esi, 3
  mov r8, rdx
  jmp .process

  .read_eax:
  mov r8, rax
  jmp .process

  .read_ebx:
  mov r8, rbx
  jmp .process

  .read_ecx:
  mov r8, rcx

  .process:
  pop rcx     ; bits
  mov eax, 1
  rol eax, cl
  xchg rax, r8
  test rax, r8

  pop rbx
  ret
