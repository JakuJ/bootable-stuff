section .gdt

global GDT32_Code
global GDT32_Data
global GDT32_Pointer

global GDT64_Code
global GDT64_Data
global GDT64_Pointer

GDT32:                  ; Global Descriptor Table (32-bit)
  .Null: equ $ - GDT64  ; The null descriptor
  dq 0x0
  GDT32_Code: equ $ - GDT32    ; The code descriptor
  dw 0xffff                    ; Limit (low)
  dw 0x0                       ; Base (low)
  db 0x0                       ; Base (middle)
  db 0b10011010                ; Access
  db 0b11001111                ; Flags, Limit 16:19
  db 0x0                       ; Base (high)
  GDT32_Data: equ $ - GDT32    ; The data descriptor
  dw 0xffff                    ; Limit (low)
  dw 0x0                       ; Base (low)
  db 0x0                       ; Base (middle)
  db 0b10010010                ; Access
  db 0b11001111                ; Flags, Limit 16:19
  db 0x0                       ; Base (high)
  GDT32_Pointer:               ; The GDT-pointer
  dw $ - GDT32 - 1             ; Size
  dd GDT32                     ; Offset

GDT64:                         ; Global Descriptor Table (64-bit)
  .Null: equ $ - GDT64         ; The null descriptor
  dw 0xFFFF                    ; Limit (low)
  dw 0                         ; Base (low)
  db 0                         ; Base (middle)
  db 0                         ; Access
  db 1                         ; Granularity
  db 0                         ; Base (high)
  GDT64_Code: equ $ - GDT64    ; The code descriptor
  dw 0                         ; Limit (low)
  dw 0                         ; Base (low)
  db 0                         ; Base (middle)
  db 10011010b                 ; Access (exec/read)
  db 10101111b                 ; Granularity, 64 bits flag, limit19:16
  db 0                         ; Base (high)
  GDT64_Data: equ $ - GDT64    ; The data descriptor
  dw 0                         ; Limit (low)
  dw 0                         ; Base (low)
  db 0                         ; Base (middle)
  db 10010010b                 ; Access (read/write)
  db 00000000b                 ; Granularity
  db 0                         ; Base (high)
  GDT64_Pointer:               ; The GDT-pointer
  dw $ - GDT64 - 1             ; Size
  dq GDT64                     ; Offset
