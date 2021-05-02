section .text

%assign i 0

%macro defhandler 0
  extern irq%[i]_handler
  global irq%[i]
  irq%[i]:
      call irq%[i]_handler
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
  sti
  ret