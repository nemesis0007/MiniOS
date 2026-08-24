#include "programs.h"

volatile unsigned int prime_result;

static int is_prime(unsigned int value)
{
    unsigned int divisor;

    if (value < 2) {
        return 0;
    }
    for (divisor = 2; divisor * divisor <= value; ++divisor) {
        if (value % divisor == 0) {
            return 0;
        }
    }
    return 1;
}

void program_prime(void)
{
    unsigned int candidate;

    prime_result = 0;
    while (program_has_cpu_work() != 0) {
        for (candidate = 2; candidate <= 1500; ++candidate) {
            if (is_prime(candidate) != 0) {
                prime_result = candidate;
            }
        }
    }
}
