%macro push_all 0
  push rdi
  push rsi
  push rbp
  push rsp
  push rbx
  push rdx
  push rcx
  push rax
  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15
%endmacro

%macro pop_all 0
  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop rax
  pop rcx
  pop rdx
  pop rbx
  pop rsp
  pop rbp
  pop rsi
  pop rdi
%endmacro
