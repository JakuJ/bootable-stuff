#pragma once

extern uintptr_t GDT64_Code;

extern int load_idt();

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

typedef struct {
    uint16_t offset_1; // offset bits 0..15
    uint16_t selector; // a code segment selector in GDT or LDT
    uint8_t ist;       // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
    uint8_t type_attr; // type and attributes
    uint16_t offset_2; // offset bits 16..31
    uint32_t offset_3; // offset bits 32..63
    uint32_t zero;     // reserved
} IDT_entry;

static IDT_entry table[256];

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) IDT_pointer;

IDT_pointer idt_ptr;

void register_with(int(*irq)(), int index, uint8_t flags) {
    intptr_t irq_address = (intptr_t) irq;

    table[index] = (IDT_entry) {
            .offset_1 = irq_address & 0xffff,
            .offset_2 = (irq_address >> 16) & 0xfff,
            .offset_3 = (irq_address >> 32) & 0xffffffff,
            .selector = (uintptr_t)(&GDT64_Code),
            .type_attr = flags,
    };
}

void register_isr(int(*irq)(), int index) {
    register_with(irq, index, 0x8f);
}

void register_irq(int(*irq)(), int index) {
    register_with(irq, index, 0x8e);
}

void IDT_init(void) {
    // Register error interrupt handlers
    register_isr(isr0, 0);
    register_isr(isr1, 1);
    register_isr(isr2, 2);
    register_isr(isr3, 3);
    register_isr(isr4, 4);
    register_isr(isr5, 5);
    register_isr(isr6, 6);
    register_isr(isr7, 7);
    register_isr(isr8, 8);
    register_isr(isr9, 9);
    register_isr(isr10, 10);
    register_isr(isr11, 11);
    register_isr(isr12, 12);
    register_isr(isr13, 13);
    register_isr(isr14, 14);
    register_isr(isr15, 15);
    register_isr(isr16, 16);
    register_isr(isr17, 17);
    register_isr(isr18, 18);

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
    uint64_t idt_address = (uint64_t) table;
    idt_ptr.limit = (sizeof(IDT_entry) * 256) - 1;
    idt_ptr.base = idt_address;

    // Load the IDT
    load_idt();
}
