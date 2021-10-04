#pragma once

void PIC_send_EOI(unsigned irq_no);

void PIC_remap(int offset1, int offset2);

void IRQ_set_mask(unsigned char IRQline);

void IRQ_clear_mask(unsigned char IRQline);
