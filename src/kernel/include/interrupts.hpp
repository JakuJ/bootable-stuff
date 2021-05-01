#pragma once

struct Registers {
    uint32_t ds; // Data segment selector
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code; // Interrupt number and error code (if applicable)
    uint32_t eip, cs, e_flags, user_esp, ss; // Pushed by the processor automatically.
};

bool are_interrupts_enabled();
