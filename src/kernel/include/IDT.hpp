#pragma once

#define KERNEL_CODE_SEGMENT_OFFSET  0x08
#define INTERRUPT_GATE              0x8e

namespace IDT {
    namespace _internal {
        extern "C"
        {
        extern int load_idt(uint32_t *);
        extern int isr0();
        extern int isr1();
        extern int isr2();
        extern int isr3();
        extern int isr4();
        extern int isr5();
        extern int isr6();
        extern int isr7();
        extern int isr8();
        extern int isr9();
        extern int isr10();
        extern int isr11();
        extern int isr12();
        extern int isr13();
        extern int isr14();
        extern int isr15();
        extern int isr16();
        extern int isr17();
        extern int isr18();
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

        // Register error interrupt handlers
        register_irq(isr0, 0);
        register_irq(isr1, 1);
        register_irq(isr2, 2);
        register_irq(isr3, 3);
        register_irq(isr4, 4);
        register_irq(isr5, 5);
        register_irq(isr6, 6);
        register_irq(isr7, 7);
        register_irq(isr8, 8);
        register_irq(isr9, 9);
        register_irq(isr10, 10);
        register_irq(isr11, 11);
        register_irq(isr12, 12);
        register_irq(isr13, 13);
        register_irq(isr14, 14);
        register_irq(isr15, 15);
        register_irq(isr16, 16);
        register_irq(isr17, 17);
        register_irq(isr18, 18);

        // Register other interrupt handlers
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
