#ifndef MINIOS_INTERRUPT_H
#define MINIOS_INTERRUPT_H

void interrupt_initialize(void);
void interrupt_enable(void);
void interrupt_disable(void);
void interrupt_send_eoi(unsigned char irq);

#endif

