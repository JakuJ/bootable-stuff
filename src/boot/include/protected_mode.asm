bits 32

protected_mode:
  call check_cpuid
  call check_long_mode
  call setup_paging
  call enable_sse

  lgdt [GDT64_Pointer]           ; Load the 64-bit GDT
  jmp GDT64_Code:long_mode       ; Enter 64-bit long mode.

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

extern PML4T
extern PDPT
extern PDT
extern PT
setup_paging:
  ; Save page hierarchy root (PML4T) to CR3
  mov edi, PML4T
  mov cr3, edi

  ; Set up tables
  ; PML4T[0] -> PDPT
  ; PDPT[0] -> PDT
  ; PDT[0] -> PT
  mov eax, PDPT
  or eax, 3
  mov dword [PML4T], eax

  mov eax, PDT
  or eax, 3
  mov dword [PDPT], eax

  mov eax, PT
  or eax, 3
  mov dword [PDT], eax

  ; Identity map the first 2MB
  mov ebx, 0x00000003          ; Physical address | flags
  mov ecx, 512                 ; Iteration counter
  mov edi, PT                  ; Pointer to the current PT cell
  .set_entry:
    mov dword [edi], ebx
    add ebx, 0x1000            ; Move to the next page
    add edi, 8                 ; Move to the next PT cell
    loop .set_entry

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
