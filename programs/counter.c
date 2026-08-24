#include "programs.h"

volatile unsigned int counter_result;

void program_counter(void)
{
    unsigned int value;

    counter_result = 0;
    while (program_has_cpu_work() != 0) {
        for (value = 1; value <= 20000; ++value) {
            counter_result += value;
        }
    }
}
