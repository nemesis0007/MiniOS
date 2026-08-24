#include "timer.h"
#include "interrupt.h"
#include "io.h"
#include "dispatcher.h"
#include "scheduler.h"

enum {
    PIT_INPUT_FREQUENCY = 1193182,
    PIT_COMMAND_PORT = 0x43,
    PIT_CHANNEL0_PORT = 0x40
};

static volatile unsigned int tick_count;
static unsigned int configured_frequency;

void timer_initialize(unsigned int frequency_hz)
{
    unsigned int divisor;

    if (frequency_hz < 19) {
        frequency_hz = 19;
    } else if (frequency_hz > 1000) {
        frequency_hz = 1000;
    }

    configured_frequency = frequency_hz;
    tick_count = 0;
    divisor = PIT_INPUT_FREQUENCY / frequency_hz;
    io_out8(PIT_COMMAND_PORT, 0x36);
    io_out8(PIT_CHANNEL0_PORT, (unsigned char)(divisor & 0xFFU));
    io_out8(PIT_CHANNEL0_PORT, (unsigned char)((divisor >> 8) & 0xFFU));
}

void timer_interrupt_handler(void)
{
    process_control_block_t *next;

    ++tick_count;
    interrupt_send_eoi(0);
    next = scheduler_on_timer_tick();
    if (next != 0) {
        dispatcher_preempt(next);
    }
}

unsigned int timer_ticks(void)
{
    return tick_count;
}

unsigned int timer_frequency(void)
{
    return configured_frequency;
}
