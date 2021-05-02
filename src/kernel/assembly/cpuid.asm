section .text

global query_cpu:
query_cpu:
  push rbx
  push rdx

  mov eax, edi  ; 1st argument: CPUID page
  mov ecx, 0
  cpuid

  cmp esi, 0    ; 2nd argument: register (0 - EAX ... 3 - EDX)
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
  pop rcx     ; 3rd argument: bit index
  mov eax, 1
  rol eax, cl
  xchg rax, r8
  test rax, r8

  pop rbx
  ret

global enable_avx
enable_avx:         ; CPU must support XSAVE
  push rax
  push rcx
  push rdx

  xor rcx, rcx
  xgetbv            ; load the XCR0 register
  or eax, 7         ; set the AVX, SSE, x87 bits
  xsetbv            ; save back to XCR0

  pop rdx
  pop rcx
  pop rax
  ret