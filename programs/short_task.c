#include "programs.h"

volatile unsigned int short_task_result;

void program_short_task(void)
{
    unsigned int index;
    unsigned int value = 0;

    while (program_has_cpu_work() != 0) {
        for (index = 0; index < 5000; ++index) {
            value = value * 33U + index;
        }
    }
    short_task_result = value;
}
