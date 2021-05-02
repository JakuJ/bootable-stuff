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
  jz no_cpuid
  ret

check_long_mode:
  mov eax, 0x80000000          ; Set the A-register to 0x80000000.
  cpuid                        ; CPU identification.
  cmp eax, 0x80000001          ; Compare the A-register with 0x80000001.
  jb no_long_mode              ; It is less, there is no long mode.
  mov eax, 0x80000001          ; Set the A-register to 0x80000001.
  cpuid                        ; CPU identification.
  test edx, 1 << 29            ; Test if the LM-bit, which is bit 29, is set in the D-register.
  jz no_long_mode              ; They aren't, there is no long mode.
  ret

%define page_table 0x1000

setup_paging:
  ; Clear the tables
  mov edi, page_table          ; Set the destination index to page_table.
  mov cr3, edi                 ; Set control register 3 to the destination index.
  xor eax, eax                 ; Nullify the A-register.
  mov ecx, 4096                ; Set the C-register to 4096.
  rep stosd                    ; Clear the memory.
  mov edi, cr3                 ; Set the destination index to control register 3.

  ; Set up tables
  ; PML4T[0] -> PDPT.
  ; PDPT[0] -> PDT.
  ; PDT[0] -> PT.
  ; PT -> 0x00000000 - 0x00200000.
  mov eax, page_table
  or eax, 3

  add eax, 0x1000
  mov DWORD [edi], eax         ; Set the uint32_t at the destination index to 0x2003.
  add edi, 0x1000              ; Add 0x1000 to the destination index.

  add eax, 0x1000
  mov DWORD [edi], eax         ; Set the uint32_t at the destination index to 0x3003.
  add edi, 0x1000              ; Add 0x1000 to the destination index.

  add eax, 0x1000
  mov DWORD [edi], eax         ; Set the uint32_t at the destination index to 0x4003.
  add edi, 0x1000              ; Add 0x1000 to the destination index.

  ; Identity map the first 2MB
  mov ebx, 0x00000003          ; Set the B-register to 0x00000003.
  mov ecx, 512                 ; Set the C-register to 512 (number of entries in PT).

  .set_entry:
    mov DWORD [edi], ebx       ; Set the uint32_t at the destination index to the B-register.
    add ebx, page_table        ; Add page_table to the B-register.
    add edi, 8                 ; Add eight to the destination index.
    loop .set_entry            ; Set the next entry.

  ; Enable PAE (Physical Address Extension)
  mov eax, cr4                 ; Set the A-register to control register 4.
  or eax, 1 << 5               ; Set the PAE-bit, which is the 6th bit (bit 5).
  mov cr4, eax                 ; Set control register 4 to the A-register.

  ; Enable long mode
  mov ecx, 0xC0000080          ; Set the C-register to 0xC0000080, which is the EFER MSR.
  rdmsr                        ; Read from the model-specific register.
  or eax, 1 << 8               ; Set the LM-bit which is the 9th bit (bit 8).
  wrmsr                        ; Write to the model-specific register.

  ; Enable paging
  mov eax, cr0                 ; Set the A-register to control register 0.
  or eax, 1 << 31              ; Set the PG-bit, which is the 32nd bit (bit 31).
  mov cr0, eax                 ; Set control register 0 to the A-register.
  ret

enable_sse:
  ; Check if SSE is available
  mov eax, 1
  cpuid
  test edx, 1 << 25
  jz no_sse

  ; Now enable SSE and the like
  mov eax, cr0
  and ax, 0xFFFB		; clear coprocessor emulation CR0.EM
  or ax, 0x2			  ; set coprocessor monitoring CR0.MP
  mov cr0, eax

  mov eax, cr4
  or eax, 3 << 9		  ; set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
  or eax, 1 << 18     ; set CR4.OSXSAVE flag to enable XSAVE (in turn used to enable AVX)
  mov cr4, eax
  ret

no_cpuid:
  mov al, 'C'
  jmp error

no_long_mode:
  mov al, 'L'
  jmp error

no_sse:
  mov al, 'S'
  jmp error

error:
  ; print "ERR: X" where X is the error code
  mov dword [0xb8000], 0x4f524f45
  mov dword [0xb8004], 0x4f3a4f52
  mov dword [0xb8008], 0x4f204f20
  mov byte  [0xb800a], al
  hlt