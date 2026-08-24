#ifndef MINIOS_TIMER_H
#define MINIOS_TIMER_H

void timer_initialize(unsigned int frequency_hz);
void timer_interrupt_handler(void);
unsigned int timer_ticks(void);
unsigned int timer_frequency(void);

#endif

