%ifidn __BITS__,64

%macro push_all 0
  push rdi
  push rsi
  push rbp
  push rsp
  push rbx
  push rdx
  push rcx
  push rax
%endmacro

%macro pop_all 0
  pop rax
  pop rcx
  pop rdx
  pop rbx
  pop rsp
  pop rbp
  pop rsi
  pop rdi
%endmacro

%else

%define push_all pusha
%define pop_all popa

%endif
