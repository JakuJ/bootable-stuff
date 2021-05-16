; Optimized SSE2 memcpy
; Input must be aligned to 16-byte boundary.
; Count must not be zero and must be a multiple of 128 bytes.
; IN\ rdi - destination
; IN\ rsi - source
; IN\ rdx - count in bytes
global kmemcpy_128
kmemcpy_128:
  shr rdx, 7      ; divide by 128 (8 * 128-bit registers = 8 * 16 = 128 bytes)

.loop_128:
  ;SSE2 prefetch
  prefetchnta [rsi + 128]
  prefetchnta [rsi + 160]
  prefetchnta [rsi + 192]
  prefetchnta [rsi + 224]

  ;move data from src to registers
  movdqa xmm0, [rsi]
  movdqa xmm1, [rsi + 16]
  movdqa xmm2, [rsi + 32]
  movdqa xmm3, [rsi + 48]
  movdqa xmm4, [rsi + 64]
  movdqa xmm5, [rsi + 80]
  movdqa xmm6, [rsi + 96]
  movdqa xmm7, [rsi + 112]

  ;move data from registers to dest
  movntdq [rdi], xmm0
  movntdq [rdi + 16], xmm1
  movntdq [rdi + 32], xmm2
  movntdq [rdi + 48], xmm3
  movntdq [rdi + 64], xmm4
  movntdq [rdi + 80], xmm5
  movntdq [rdi + 96], xmm6
  movntdq [rdi + 112], xmm7

  add rsi, 128
  add rdi, 128

  dec rdx
  jnz .loop_128
  ret

global kmemcpy
kmemcpy:
  mov rcx, rdx    ; rcx is out counter
  shr rcx, 7      ; divide by 128 (8 * 128-bit registers = 8 * 16 = 128 bytes)
  jz .end_loop_128

.loop_128:
  ;SSE2 prefetch
  prefetchnta [rsi + 128]
  prefetchnta [rsi + 160]
  prefetchnta [rsi + 192]
  prefetchnta [rsi + 224]

  ;move data from src to registers
  movdqa xmm0, [rsi]
  movdqa xmm1, [rsi + 16]
  movdqa xmm2, [rsi + 32]
  movdqa xmm3, [rsi + 48]
  movdqa xmm4, [rsi + 64]
  movdqa xmm5, [rsi + 80]
  movdqa xmm6, [rsi + 96]
  movdqa xmm7, [rsi + 112]

  ;move data from registers to dest
  movntdq [rdi], xmm0
  movntdq [rdi + 16], xmm1
  movntdq [rdi + 32], xmm2
  movntdq [rdi + 48], xmm3
  movntdq [rdi + 64], xmm4
  movntdq [rdi + 80], xmm5
  movntdq [rdi + 96], xmm6
  movntdq [rdi + 112], xmm7

  add rsi, 128
  add rdi, 128

  loop .loop_128
.end_loop_128:

  and rdx, 0x07f   ; the rest, up to 128 bytes
  mov rcx, rdx
  shr rcx, 4
  jz .end_loop_16

.loop_16:
  movdqa xmm0, [rsi]
  movntdq [rdi], xmm0

  add rsi, 16
  add rdi, 16

  loop .loop_16
.end_loop_16:

  and rdx, 0x0f   ; the rest, up to 16 bytes
  mov rcx, rdx

  rep movsb

  ret