#include "telemetry.h"
#include "process.h"
#include "scheduler.h"
#include "serial.h"
#include "statistics.h"
#include "timeline.h"

static void separator(void)
{
    serial_putchar('|');
}

void telemetry_publish(void)
{
    system_statistics_t statistics = statistics_calculate();
    unsigned int index;

    serial_writeline("@MINIOS|BEGIN");
    serial_write("@MINIOS|SCHEDULER|");
    serial_write(scheduler_name());
    separator();
    serial_write_uint(scheduler_quantum());
    serial_putchar('\n');

    for (index = 0; index < process_slot_count(); ++index) {
        const process_control_block_t *process = process_at(index);

        serial_write("@MINIOS|PROCESS|");
        serial_write_uint(process->pid); separator();
        serial_write(process->name); separator();
        serial_write(process_state_name(process->state)); separator();
        serial_write_uint(process->priority); separator();
        serial_write_uint(process->arrival_time); separator();
        serial_write_uint(process->burst_time); separator();
        serial_write_uint(process->remaining_time);
        serial_putchar('\n');
    }

    for (index = 0; index < timeline_count(); ++index) {
        const timeline_event_t *event = timeline_at(index);

        serial_write("@MINIOS|EVENT|");
        serial_write_uint(event->pid); separator();
        serial_write_uint(event->start_time); separator();
        serial_write_uint(event->end_time);
        serial_putchar('\n');
    }

    serial_write("@MINIOS|STATS|");
    serial_write_uint(statistics.average_waiting_hundredths); separator();
    serial_write_uint(statistics.average_turnaround_hundredths); separator();
    serial_write_uint(statistics.average_response_hundredths); separator();
    serial_write_uint(statistics.context_switches); separator();
    serial_write_uint(statistics.cpu_utilization_hundredths); separator();
    serial_write_uint(statistics.throughput_hundredths);
    serial_putchar('\n');
    serial_writeline("@MINIOS|END");
}
