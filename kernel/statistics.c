#include "statistics.h"
#include "process.h"
#include "scheduler.h"

system_statistics_t statistics_calculate(void)
{
    system_statistics_t result = { 0 };
    unsigned int total_waiting = 0;
    unsigned int total_turnaround = 0;
    unsigned int total_response = 0;
    unsigned int index;

    for (index = 0; index < process_slot_count(); ++index) {
        const process_control_block_t *process = process_at(index);

        if (process->state != PROCESS_TERMINATED ||
            process->statistics.has_started == 0) {
            continue;
        }
        ++result.completed_processes;
        total_waiting += process->statistics.waiting_time;
        total_turnaround += process->statistics.turnaround_time;
        total_response += process->statistics.response_time;
    }

    if (result.completed_processes != 0) {
        result.average_waiting_hundredths =
            total_waiting * 100 / result.completed_processes;
        result.average_turnaround_hundredths =
            total_turnaround * 100 / result.completed_processes;
        result.average_response_hundredths =
            total_response * 100 / result.completed_processes;
    }

    result.elapsed_ticks = scheduler_clock();
    result.busy_ticks = scheduler_busy_ticks();
    result.context_switches = scheduler_context_switches();
    if (result.elapsed_ticks != 0) {
        result.cpu_utilization_hundredths =
            result.busy_ticks * 10000 / result.elapsed_ticks;
        result.throughput_hundredths =
            result.completed_processes * 100 / result.elapsed_ticks;
    }
    return result;
}

