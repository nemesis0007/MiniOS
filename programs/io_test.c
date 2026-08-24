#include "programs.h"
#include "scheduler.h"

volatile unsigned int io_test_result;

void program_io_test(void)
{
    unsigned int phase;
    unsigned int index;
    unsigned int value = 0;
    unsigned int waits = 0;
    unsigned int last_tick = scheduler_clock();

    while (program_has_cpu_work() != 0) {
        for (phase = 0; phase < 5; ++phase) {
            for (index = 0; index < 3000; ++index) {
                value += (phase + 1) * (index ^ 0x5A5AU);
            }
        }
        if (waits < 3 && scheduler_clock() > last_tick &&
            program_has_cpu_work() != 0) {
            ++waits;
            scheduler_sleep_current(2);
            last_tick = scheduler_clock();
        }
    }
    io_test_result = value;
}
