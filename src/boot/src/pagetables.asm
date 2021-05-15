section .bss
align 4096

global PML4T
global PDPT
global PDT
global PT

PML4T:
resb 512 * 8
PDPT:
resb 512 * 8
PDT:
resb 512 * 8
PT:
resb 512 * 8
