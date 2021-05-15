section .bss
align 4096

global PML4T
global PDPT
global PDT
global PT1
global PT2

PML4T:
resb 512 * 8
PDPT:
resb 512 * 8
PDT:
resb 512 * 8
PT1:
resb 512 * 8
PT2:
resb 512 * 8
