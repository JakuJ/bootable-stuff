section .text

%assign i 0

%macro defhandler 0
  extern irq%[i]_handler
  global irq%[i]
  irq%[i]:
      pusha
      call irq%[i]_handler
      popa
      iret
  %assign i i+1
%endmacro

%rep 16
  defhandler
%endrep

global load_idt
load_idt:
	mov edx, [esp + 4]  ; load argument into edx
	lidt [edx]          ; load IDT
	sti                 ; enable interrupts
	ret