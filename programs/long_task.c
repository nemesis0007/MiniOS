#include "programs.h"

volatile unsigned int long_task_result;

void program_long_task(void)
{
    unsigned int index;
    unsigned int value = 0x12345678U;

    while (program_has_cpu_work() != 0) {
        for (index = 0; index < 20000; ++index) {
            value = (value << 5) ^ (value >> 3) ^ index;
        }
    }
    long_task_result = value;
}
