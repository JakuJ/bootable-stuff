section .gdt

global GDT32_Code
global GDT32_Data
global GDT32_Pointer

global GDT64_Code
global GDT64_Code_User
global GDT64_Data
global GDT64_Data_User
global GDT64_TSS
global GDT64_TSS_ADDR
global GDT64_Pointer

GDT32:                              ; Global Descriptor Table (32-bit)
  .Null: equ $ - GDT64              ; The null descriptor
  dq 0x0

  GDT32_Code: equ $ - GDT32         ; The code descriptor for ring 0
  dw 0xffff                         ; Limit (low)
  dw 0x0                            ; Base (low)
  db 0x0                            ; Base (middle)
  db 0b10011010                     ; Access
  db 0b11001111                     ; Flags, Limit 16:19
  db 0x0                            ; Base (high)

  GDT32_Data: equ $ - GDT32         ; The data descriptor for ring 0
  dw 0xffff                         ; Limit (low)
  dw 0x0                            ; Base (low)
  db 0x0                            ; Base (middle)
  db 0b10010010                     ; Access
  db 0b11001111                     ; Flags, Limit 16:19
  db 0x0                            ; Base (high)

  GDT32_Pointer:                    ; The GDT pointer
  dw $ - GDT32 - 1                  ; Size
  dd GDT32                          ; Offset

GDT64:                              ; Global Descriptor Table (64-bit)
  .Null: equ $ - GDT64              ; The null descriptor
  dq 0

  GDT64_Code: equ $ - GDT64         ; The code descriptor for ring 0
  dw 0xffff                         ; Limit (low)
  dw 0                              ; Base (low)
  db 0                              ; Base (middle)
  db 10011010b                      ; Access (exec/read)
  db 10101111b                      ; Flags, limit 19:16
  db 0                              ; Base (high)

  GDT64_Data: equ $ - GDT64         ; The data descriptor for ring 0
  dw 0xffff                         ; Limit (low)
  dw 0                              ; Base (low)
  db 0                              ; Base (middle)
  db 10010010b                      ; Access (read/write)
  db 10001111b                      ; Flags, limit 19:16
  db 0                              ; Base (high)

  GDT64_Data_User: equ $ - GDT64    ; The data descriptor for ring 3
  dw 0xffff                         ; Limit (low)
  dw 0                              ; Base (low)
  db 0                              ; Base (middle)
  db 11110010b                      ; Access (read/write)
  db 10001111b                      ; Flags, limit 19:16
  db 0                              ; Base (high)

  GDT64_Code_User: equ $ - GDT64    ; The code descriptor for ring 3
  dw 0xffff                         ; Limit (low)
  dw 0                              ; Base (low)
  db 0                              ; Base (middle)
  db 11111010b                      ; Access (exec/read)
  db 10101111b                      ; Flags, limit 19:16
  db 0                              ; Base (high)

  GDT64_TSS: equ $ - GDT64          ; The task state segment descriptor
  GDT64_TSS_ADDR:
  dq 0
  dq 0

  GDT64_Pointer:                    ; The GDT pointer
  dw $ - GDT64 - 1                  ; Size
  dq GDT64                          ; Offset
