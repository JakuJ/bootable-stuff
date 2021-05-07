#pragma once

void PIC_send_EOI(unsigned irq_no);

void PIC_remap(int offset1, int offset2);
