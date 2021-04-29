#pragma once

#define KERNEL_CODE_SEGMENT_OFFSET  0x08
#define INTERRUPT_GATE              0x8e

namespace IDT {
    namespace _internal {
        extern "C"
        {
        extern int load_idt(uint32_t *);
        extern int irq0();
        extern int irq1();
        extern int irq2();
        extern int irq3();
        extern int irq4();
        extern int irq5();
        extern int irq6();
        extern int irq7();
        extern int irq8();
        extern int irq9();
        extern int irq10();
        extern int irq11();
        extern int irq12();
        extern int irq13();
        extern int irq14();
        extern int irq15();
        }

        struct IDT_entry {
            unsigned short int offset_lowerbits;
            unsigned short int selector;
            unsigned char zero;
            unsigned char type_attr;
            unsigned short int offset_higherbits;
        };

        static IDT_entry table[256];

        void register_irq(int(*irq)(), int index) {
            uint32_t irq_address = (uint32_t) irq;
            table[index].offset_lowerbits = irq_address & 0xffff;
            table[index].selector = KERNEL_CODE_SEGMENT_OFFSET;
            table[index].zero = 0;
            table[index].type_attr = INTERRUPT_GATE;
            table[index].offset_higherbits = (irq_address & 0xffff0000) >> 16;
        }
    }

    void init() {
        using namespace _internal;

        // Register handlers
        register_irq(irq0, 32);
        register_irq(irq1, 33);
        register_irq(irq2, 34);
        register_irq(irq3, 35);
        register_irq(irq4, 36);
        register_irq(irq5, 37);
        register_irq(irq6, 38);
        register_irq(irq7, 39);
        register_irq(irq8, 40);
        register_irq(irq9, 41);
        register_irq(irq10, 42);
        register_irq(irq11, 43);
        register_irq(irq12, 44);
        register_irq(irq13, 45);
        register_irq(irq14, 46);
        register_irq(irq15, 47);

        // Fill the IDT descriptor
        uint32_t idt_address = (uint32_t) table;

        uint32_t idt_ptr[2];
        idt_ptr[0] = (sizeof(IDT_entry) * 256) + ((idt_address & 0xffff) << 16);
        idt_ptr[1] = idt_address >> 16;

        // Load the IDT
        load_idt(idt_ptr);
    }
}
