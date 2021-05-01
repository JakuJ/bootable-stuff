check_cpuid:
  ; Check if CPUID is supported by attempting to flip the ID bit (bit 21) in
  ; the FLAGS register. If we can flip it, CPUID is available.

  ; Copy FLAGS in to EAX via stack
  pushfd
  pop eax

  ; Copy to ECX as well for comparing later on
  mov ecx, eax

  ; Flip the ID bit
  xor eax, 1 << 21

  ; Copy EAX to FLAGS via the stack
  push eax
  popfd

  ; Copy FLAGS back to EAX (with the flipped bit if CPUID is supported)
  pushfd
  pop eax

  ; Restore FLAGS from the old version stored in ECX (i.e. flipping the ID bit
  ; back if it was ever flipped).
  push ecx
  popfd

  ; Compare EAX and ECX. If they are equal then that means the bit wasn't
  ; flipped, and CPUID isn't supported.
  xor eax, ecx
  jz NoCPUID
  ret

check_long_mode:
  mov eax, 0x80000000    ; Set the A-register to 0x80000000.
  cpuid                  ; CPU identification.
  cmp eax, 0x80000001    ; Compare the A-register with 0x80000001.
  jb NoLongMode          ; It is less, there is no long mode.
  mov eax, 0x80000001    ; Set the A-register to 0x80000001.
  cpuid                  ; CPU identification.
  test edx, 1 << 29      ; Test if the LM-bit, which is bit 29, is set in the D-register.
  jz NoLongMode         ; They aren't, there is no long mode.
  ret

setup_paging:
  mov edi, 0x1000    ; Set the destination index to 0x1000.
  mov cr3, edi       ; Set control register 3 to the destination index.
  xor eax, eax       ; Nullify the A-register.
  mov ecx, 4096      ; Set the C-register to 4096.
  rep stosd          ; Clear the memory.
  mov edi, cr3       ; Set the destination index to control register 3.

  mov DWORD [edi], 0x2003      ; Set the uint32_t at the destination index to 0x2003.
  add edi, 0x1000              ; Add 0x1000 to the destination index.
  mov DWORD [edi], 0x3003      ; Set the uint32_t at the destination index to 0x3003.
  add edi, 0x1000              ; Add 0x1000 to the destination index.
  mov DWORD [edi], 0x4003      ; Set the uint32_t at the destination index to 0x4003.
  add edi, 0x1000              ; Add 0x1000 to the destination index.

  mov ebx, 0x00000003          ; Set the B-register to 0x00000003.
  mov ecx, 512                 ; Set the C-register to 512.

  .SetEntry:
    mov DWORD [edi], ebx         ; Set the uint32_t at the destination index to the B-register.
    add ebx, 0x1000              ; Add 0x1000 to the B-register.
    add edi, 8                   ; Add eight to the destination index.
    loop .SetEntry               ; Set the next entry.

  mov eax, cr4                 ; Set the A-register to control register 4.
  or eax, 1 << 5               ; Set the PAE-bit, which is the 6th bit (bit 5).
  mov cr4, eax                 ; Set control register 4 to the A-register.

  mov ecx, 0xC0000080          ; Set the C-register to 0xC0000080, which is the EFER MSR.
  rdmsr                        ; Read from the model-specific register.
  or eax, 1 << 8               ; Set the LM-bit which is the 9th bit (bit 8).
  wrmsr                        ; Write to the model-specific register.

  mov eax, cr0                 ; Set the A-register to control register 0.
  or eax, 1 << 31              ; Set the PG-bit, which is the 32nd bit (bit 31).
  mov cr0, eax                 ; Set control register 0 to the A-register.

NoCPUID:
NoLongMode:
  ret ; TODO: Error handling