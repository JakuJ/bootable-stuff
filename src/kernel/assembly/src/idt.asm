section .text

%include "src/kernel/assembly/include/push_all.asm"

%assign i 0

%macro defhandler 0
  extern irq%[i]_handler
  global irq%[i]
  irq%[i]:
      push_all
      call irq%[i]_handler
      pop_all

      ; TODO: why is CS 0x2b instead of (GDT64_Code_User OR 3) ?
      extern GDT64_Code_User
      mov eax, GDT64_Code_User
      or eax, 3
      mov dword [rsp + 8], eax ; filthy hack

      iretq
  %assign i i+1
%endmacro

%rep 16
  defhandler
%endrep

global load_idt
load_idt:
  extern idt_ptr
  lidt [idt_ptr]
  ret