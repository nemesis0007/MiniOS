#ifndef MINIOS_STATISTICS_H
#define MINIOS_STATISTICS_H

typedef struct system_statistics {
    unsigned int completed_processes;
    unsigned int average_waiting_hundredths;
    unsigned int average_turnaround_hundredths;
    unsigned int average_response_hundredths;
    unsigned int cpu_utilization_hundredths;
    unsigned int throughput_hundredths;
    unsigned int context_switches;
    unsigned int elapsed_ticks;
    unsigned int busy_ticks;
} system_statistics_t;

system_statistics_t statistics_calculate(void);

#endif

