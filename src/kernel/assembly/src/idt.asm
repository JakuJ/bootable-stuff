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