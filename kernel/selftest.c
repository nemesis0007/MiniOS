#include "selftest.h"
#include "process.h"
#include "programs.h"
#include "scheduler.h"
#include "serial.h"
#include "statistics.h"
#include "terminal.h"
#include "timeline.h"

static unsigned int passed_count;
static unsigned int failed_count;

static void report(const char *name, int passed)
{
    terminal_write(passed ? "[PASS] " : "[FAIL] ");
    terminal_writeline(name);
    serial_write("@TEST|");
    serial_write(passed ? "PASS|" : "FAIL|");
    serial_writeline(name);
    if (passed) {
        ++passed_count;
    } else {
        ++failed_count;
    }
}

static process_control_block_t *create_test_process(const char *name,
                                                     unsigned int priority,
                                                     unsigned int burst)
{
    process_control_block_t *process = process_create(
        name, priority, 0, burst, program_short_task);

    if (process == 0 || process_admit(process->pid) == 0) {
        return 0;
    }
    return process;
}

static void reset_test(const char *algorithm, unsigned int quantum)
{
    process_initialize();
    scheduler_initialize();
    scheduler_select_name(algorithm);
    scheduler_set_quantum(quantum);
}

int selftest_run_all(void)
{
    process_control_block_t *first;
    process_control_block_t *second;
    system_statistics_t statistics;
    const timeline_event_t *event;
    int passed;

    passed_count = 0;
    failed_count = 0;
    terminal_writeline("MiniOS kernel integration tests");
    terminal_writeline("-------------------------------");

    report("process creation, admission, termination, states",
           process_self_test());

    reset_test("fcfs", 2);
    report("zero-process scheduler", scheduler_start() == 0 &&
           timeline_count() == 0);

    reset_test("fcfs", 2);
    first = create_test_process("one", 1, 1);
    passed = first != 0 && scheduler_start() == 1 &&
             first->state == PROCESS_TERMINATED;
    statistics = statistics_calculate();
    passed = passed && statistics.average_waiting_hundredths == 0 &&
             statistics.average_turnaround_hundredths == 100 &&
             statistics.average_response_hundredths == 0;
    report("one process and timing metrics", passed);

    reset_test("fcfs", 2);
    first = create_test_process("first", 3, 2);
    second = create_test_process("second", 1, 1);
    scheduler_start();
    event = timeline_at(0);
    passed = first != 0 && second != 0 && timeline_count() == 2 &&
             event != 0 && event->pid == first->pid &&
             second->statistics.waiting_time == 2 &&
             second->statistics.turnaround_time == 3 &&
             second->statistics.response_time == 2;
    report("FCFS order and wait/turnaround/response", passed);

    reset_test("sjf", 2);
    first = create_test_process("longer", 1, 3);
    second = create_test_process("shorter", 1, 1);
    scheduler_start();
    event = timeline_at(0);
    statistics = statistics_calculate();
    report("SJF order and equal arrivals", first != 0 && second != 0 &&
           event != 0 && event->pid == second->pid &&
           statistics.average_waiting_hundredths == 50);

    reset_test("priority", 2);
    first = create_test_process("low", 4, 1);
    second = create_test_process("high", 1, 1);
    scheduler_start();
    event = timeline_at(0);
    report("priority order", first != 0 && second != 0 && event != 0 &&
           event->pid == second->pid);

    reset_test("sjf", 2);
    first = create_test_process("tie-a", 2, 1);
    second = create_test_process("tie-b", 2, 1);
    scheduler_start();
    event = timeline_at(0);
    report("same burst tie breaks by PID", first != 0 && second != 0 &&
           event != 0 && event->pid == first->pid);

    reset_test("priority", 2);
    first = create_test_process("tie-a", 2, 1);
    second = create_test_process("tie-b", 2, 1);
    scheduler_start();
    event = timeline_at(0);
    report("same priority tie breaks by PID", first != 0 && second != 0 &&
           event != 0 && event->pid == first->pid);

    reset_test("rr", 2);
    first = create_test_process("rr-a", 1, 3);
    second = create_test_process("rr-b", 1, 3);
    scheduler_start();
    passed = first != 0 && second != 0 && timeline_count() == 4 &&
             timeline_at(0)->pid == first->pid &&
             timeline_at(0)->end_time == 2 &&
             timeline_at(1)->pid == second->pid &&
             timeline_at(1)->end_time == 4 &&
             scheduler_context_switches() == 4;
    report("Round Robin rotation, quantum, context switches", passed);

    reset_test("rr", 10);
    first = create_test_process("tiny-a", 1, 2);
    second = create_test_process("tiny-b", 1, 1);
    scheduler_start();
    report("quantum larger than burst", first != 0 && second != 0 &&
           timeline_count() == 2 && scheduler_context_switches() == 2);

    reset_test("fcfs", 2);
    passed = scheduler_select_name("sjf") && scheduler_select_name("priority") &&
             scheduler_select_name("rr") && scheduler_select_name("fcfs") &&
             !scheduler_select_name("invalid");
    report("scheduler switching", passed);

    reset_test("fcfs", 2);
    create_test_process("multi-a", 1, 1);
    create_test_process("multi-b", 1, 1);
    create_test_process("multi-c", 1, 1);
    report("multiple simultaneous processes", scheduler_start() == 3 &&
           statistics_calculate().completed_processes == 3);

    reset_test("fcfs", 2);
    first = process_create("io-wait", 1, 0, 5, program_io_test);
    if (first != 0) {
        process_admit(first->pid);
    }
    scheduler_start();
    statistics = statistics_calculate();
    report("I/O process WAITING and idle accounting", first != 0 &&
           first->state == PROCESS_TERMINATED && first->blocked_time >= 2 &&
           statistics.cpu_utilization_hundredths < 10000);

    terminal_write("Summary: "); terminal_write_uint(passed_count);
    terminal_write(" passed, "); terminal_write_uint(failed_count);
    terminal_writeline(" failed.");
    serial_write("@TEST|SUMMARY|"); serial_write_uint(passed_count);
    serial_putchar('|'); serial_write_uint(failed_count); serial_putchar('\n');

    process_initialize();
    scheduler_initialize();
    return failed_count == 0;
}
