#include "programs.h"

volatile unsigned int fibonacci_result;

void program_fibonacci(void)
{
    unsigned int previous = 0;
    unsigned int current = 1;
    unsigned int index;

    while (program_has_cpu_work() != 0) {
        for (index = 0; index < 20000; ++index) {
            unsigned int next = previous + current;
            previous = current;
            current = next;
        }
    }
    fibonacci_result = current;
}
