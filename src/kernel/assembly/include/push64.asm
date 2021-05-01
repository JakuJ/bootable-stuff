%ifidn __BITS__,64

%macro push_all 0
  ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
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
  ; Pops edi,esi,ebp,esp,ebx,edx,ecx,eax
  push rax
  push rcx
  push rdx
  push rbx
  push rsp
  push rbp
  push rsi
  push rdi
%endmacro

%else

%define push_all pusha
%define pop_all popa

%endif
