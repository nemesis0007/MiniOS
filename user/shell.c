#include "shell.h"
#include "experiment.h"
#include "keyboard.h"
#include "process.h"
#include "programs.h"
#include "scheduler.h"
#include "selftest.h"
#include "statistics.h"
#include "telemetry.h"
#include "terminal.h"
#include "timer.h"
#include "timeline.h"

enum { SHELL_INPUT_CAPACITY = 64 };

static int strings_equal(const char *left, const char *right)
{
    unsigned int index = 0;

    while (left[index] != '\0' && right[index] != '\0') {
        if (left[index] != right[index]) {
            return 0;
        }
        ++index;
    }

    return left[index] == right[index];
}

static unsigned int parse_uint(const char *text, int *valid)
{
    unsigned int value = 0;
    unsigned int index = 0;

    *valid = 0;
    if (text == 0 || text[0] == '\0') {
        return 0;
    }

    while (text[index] != '\0') {
        if (text[index] < '0' || text[index] > '9') {
            return 0;
        }
        value = value * 10 + (unsigned int)(text[index] - '0');
        ++index;
    }

    *valid = 1;
    return value;
}

static char *next_token(char **cursor)
{
    char *token;

    while (**cursor == ' ') {
        ++*cursor;
    }

    if (**cursor == '\0') {
        return 0;
    }

    token = *cursor;
    while (**cursor != '\0' && **cursor != ' ') {
        ++*cursor;
    }

    if (**cursor == ' ') {
        **cursor = '\0';
        ++*cursor;
    }

    return token;
}

static void shell_banner(void)
{
    static int self_test_done;
    static int self_test_passed;
    static int programs_test_passed;

    if (self_test_done == 0) {
        self_test_passed = process_self_test();
        programs_test_passed = program_registry_self_test();
        self_test_done = 1;
    }

    terminal_writeline("================================");
    terminal_writeline("          MiniOS v0.19");
    terminal_writeline("================================");
    terminal_putchar('\n');
    terminal_writeline("Keyboard initialized.");
    terminal_writeline("PIT timer initialized at 100 Hz.");
    if (self_test_passed != 0) {
        terminal_writeline("Process manager self-test passed.");
    } else {
        terminal_writeline("ERROR: process manager self-test failed.");
    }
    if (programs_test_passed != 0) {
        terminal_writeline("Program registry self-test passed (6 workloads).");
    } else {
        terminal_writeline("ERROR: program registry self-test failed.");
    }
    terminal_writeline("Type 'help' for available commands.");
    terminal_putchar('\n');
}

static void print_help(void)
{
    terminal_writeline("Available commands:");
    terminal_writeline("help                 Show available commands");
    terminal_writeline("clear                Clear screen");
    terminal_writeline("about                Show MiniOS information");
    terminal_writeline("ps                   List processes");
    terminal_writeline("run <program>        Create a process");
    terminal_writeline("kill <pid>           Terminate a process");
    terminal_writeline("scheduler <name>     Select fcfs/sjf/priority/rr");
    terminal_writeline("quantum <ticks>      Set Round Robin quantum");
    terminal_writeline("start / stop         Control scheduler execution");
    terminal_writeline("stats                Show scheduling statistics");
    terminal_writeline("ticks                Show hardware timer ticks");
    terminal_writeline("timeline             Show CPU ownership history");
    terminal_writeline("benchmark <a|b>      Compare all schedulers");
    terminal_writeline("selftest             Run kernel integration tests");
}

static void print_processes(void)
{
    unsigned int index;

    terminal_writeline("PID    PROGRAM         STATE       PRIORITY");
    terminal_writeline("------------------------------------------");
    if (process_slot_count() == 0) {
        terminal_writeline("(no processes)");
        return;
    }

    for (index = 0; index < process_slot_count(); ++index) {
        const process_control_block_t *process = process_at(index);

        terminal_write_uint(process->pid);
        terminal_write("      ");
        terminal_write(process->name);
        terminal_write("          ");
        terminal_write(process_state_name(process->state));
        terminal_write("       ");
        terminal_write_uint(process->priority);
        terminal_putchar('\n');
    }
}

static void print_timing_records(void)
{
    system_statistics_t summary;
    unsigned int index;

    terminal_write("Scheduler: ");
    terminal_writeline(scheduler_name());
    terminal_writeline("PID  ARR  BURST  START  DONE  WAIT  TURN  RESP");
    terminal_writeline("-----------------------------------------------");
    for (index = 0; index < process_slot_count(); ++index) {
        const process_control_block_t *process = process_at(index);

        terminal_write_uint(process->pid); terminal_write("    ");
        terminal_write_uint(process->arrival_time); terminal_write("    ");
        terminal_write_uint(process->burst_time); terminal_write("      ");
        terminal_write_uint(process->statistics.start_time); terminal_write("      ");
        terminal_write_uint(process->statistics.completion_time); terminal_write("     ");
        terminal_write_uint(process->statistics.waiting_time); terminal_write("     ");
        terminal_write_uint(process->statistics.turnaround_time); terminal_write("     ");
        terminal_write_uint(process->statistics.response_time);
        terminal_putchar('\n');
    }
    summary = statistics_calculate();
    terminal_write("Avg waiting: ");
    terminal_write_hundredths(summary.average_waiting_hundredths);
    terminal_writeline(" ticks");
    terminal_write("Avg turnaround: ");
    terminal_write_hundredths(summary.average_turnaround_hundredths);
    terminal_writeline(" ticks");
    terminal_write("Avg response: ");
    terminal_write_hundredths(summary.average_response_hundredths);
    terminal_writeline(" ticks");
    terminal_write("CPU utilization: ");
    terminal_write_hundredths(summary.cpu_utilization_hundredths);
    terminal_writeline("%");
    terminal_write("Throughput: ");
    terminal_write_hundredths(summary.throughput_hundredths);
    terminal_writeline(" processes/tick");
    terminal_write("Context switches: ");
    terminal_write_uint(summary.context_switches);
    terminal_putchar('\n');
    if (strings_equal(scheduler_name(), "Round Robin")) {
        terminal_write("Quantum: ");
        terminal_write_uint(scheduler_quantum());
        terminal_writeline(" ticks");
    }
}

static void print_timeline(void)
{
    unsigned int index;

    terminal_write("Timeline (" );
    terminal_write(scheduler_name());
    terminal_writeline("):");
    if (timeline_count() == 0) {
        terminal_writeline("(no CPU events)");
        return;
    }
    for (index = 0; index < timeline_count(); ++index) {
        const timeline_event_t *event = timeline_at(index);

        terminal_write_uint(event->start_time);
        terminal_putchar('-');
        terminal_write_uint(event->end_time);
        terminal_write(":P");
        terminal_write_uint(event->pid);
        if (index + 1 != timeline_count()) {
            terminal_write(" | ");
        }
    }
    terminal_putchar('\n');
    if (timeline_overflowed() != 0) {
        terminal_writeline("Warning: event log capacity reached.");
    }
}

static void print_bar(unsigned int hundredths)
{
    unsigned int count = hundredths / 100;
    unsigned int index;

    if (count > 30) {
        count = 30;
    }
    for (index = 0; index < count; ++index) {
        terminal_putchar('#');
    }
}

static void print_experiment(char workload_name)
{
    unsigned int index;

    terminal_write("Workload ");
    terminal_putchar((char)(workload_name - 'a' + 'A'));
    terminal_write(": ");
    terminal_writeline(experiment_workload_description(workload_name));
    terminal_writeline("ALGORITHM     WAIT     TURN     RESP     SWITCH");
    terminal_writeline("----------------------------------------------");
    for (index = 0; index < experiment_result_count(); ++index) {
        const experiment_result_t *result = experiment_result_at(index);

        terminal_write(result->algorithm); terminal_write("        ");
        terminal_write_hundredths(result->statistics.average_waiting_hundredths);
        terminal_write("     ");
        terminal_write_hundredths(result->statistics.average_turnaround_hundredths);
        terminal_write("     ");
        terminal_write_hundredths(result->statistics.average_response_hundredths);
        terminal_write("     ");
        terminal_write_uint(result->statistics.context_switches);
        terminal_putchar('\n');
    }
    terminal_writeline("Average waiting-time bars (1 # = 1 tick):");
    for (index = 0; index < experiment_result_count(); ++index) {
        const experiment_result_t *result = experiment_result_at(index);

        terminal_write(result->algorithm);
        terminal_write(": ");
        print_bar(result->statistics.average_waiting_hundredths);
        terminal_putchar('\n');
    }
}

static void shell_execute(char *line)
{
    char *cursor = line;
    char *command = next_token(&cursor);
    char *argument = next_token(&cursor);
    char *extra = next_token(&cursor);

    if (command == 0) {
        return;
    }

    if (strings_equal(command, "help")) {
        print_help();
    } else if (strings_equal(command, "clear")) {
        terminal_clear();
        shell_banner();
    } else if (strings_equal(command, "about")) {
        terminal_writeline("MiniOS v0.19 - educational x86 operating system");
        terminal_writeline("Built-ins: counter fibonacci prime short long io_test");
    } else if (strings_equal(command, "scheduler")) {
        if (argument == 0 && extra == 0) {
            terminal_write("Current scheduler: ");
            terminal_writeline(scheduler_name());
            if (strings_equal(scheduler_name(), "Round Robin")) {
                terminal_write("Quantum: ");
                terminal_write_uint(scheduler_quantum());
                terminal_writeline(" ticks");
            }
        } else if (extra != 0) {
            terminal_writeline("Usage: scheduler <fcfs|sjf|priority|rr>");
        } else if (scheduler_select_name(argument) != 0) {
            terminal_write("Scheduler selected: ");
            terminal_writeline(scheduler_name());
        } else {
            terminal_writeline("Use fcfs, sjf, priority, or rr.");
        }
    } else if (strings_equal(command, "quantum")) {
        int valid;
        unsigned int requested = parse_uint(argument, &valid);

        if (extra != 0 || valid == 0 || requested == 0 || requested > 100) {
            terminal_writeline("Usage: quantum <1-100>");
        } else {
            scheduler_set_quantum(requested);
            terminal_write("Time quantum set to ");
            terminal_write_uint(scheduler_quantum());
            terminal_writeline(" ticks.");
        }
    } else if (strings_equal(command, "ps")) {
        print_processes();
    } else if (strings_equal(command, "kill")) {
        int valid;
        unsigned int pid = parse_uint(argument, &valid);

        if (extra != 0 || valid == 0) {
            terminal_writeline("Usage: kill <pid>");
        } else if (process_terminate(pid) != 0) {
            terminal_write("Process ");
            terminal_write_uint(pid);
            terminal_writeline(" terminated.");
        } else {
            terminal_writeline("No active process has that PID.");
        }
    } else if (strings_equal(command, "run")) {
        const program_descriptor_t *program;
        process_control_block_t *process;

        if (argument == 0 || extra != 0) {
            terminal_writeline("Usage: run <program>");
            return;
        }

        program = program_lookup(argument);
        if (program == 0) {
            terminal_writeline("Unknown program. Use about to list built-ins.");
            return;
        }

        process = process_create(program->name,
                                 program->default_priority,
                                 0,
                                 program->estimated_burst,
                                 program->entry);
        if (process == 0) {
            terminal_writeline("Process creation failed: table full or invalid metadata.");
            return;
        }
        if (process_admit(process->pid) == 0) {
            terminal_writeline("Process creation failed during admission.");
            return;
        }
        terminal_writeline("Process created.");
        terminal_write("PID: ");
        terminal_write_uint(process->pid);
        terminal_putchar('\n');
    } else if (strings_equal(command, "ticks")) {
        terminal_write("Timer ticks: ");
        terminal_write_uint(timer_ticks());
        terminal_write(" at ");
        terminal_write_uint(timer_frequency());
        terminal_writeline(" Hz.");
    } else if (strings_equal(command, "timeline")) {
        print_timeline();
    } else if (strings_equal(command, "benchmark")) {
        if (argument == 0 || argument[1] != '\0' ||
            (argument[0] != 'a' && argument[0] != 'b') || extra != 0) {
            terminal_writeline("Usage: benchmark <a|b>");
        } else {
            terminal_writeline("Running real workloads under all four schedulers...");
            if (experiment_run(argument[0]) == 0) {
                terminal_writeline("Benchmark failed.");
            } else {
                print_experiment(argument[0]);
            }
        }
    } else if (strings_equal(command, "selftest")) {
        if (argument != 0) {
            terminal_writeline("Usage: selftest");
        } else {
            selftest_run_all();
        }
    } else if (strings_equal(command, "start")) {
        unsigned int completed = scheduler_start();

        if (completed == 0) {
            terminal_writeline("No READY process to dispatch.");
        } else {
            terminal_write(scheduler_name());
            terminal_write(" completed ");
            terminal_write_uint(completed);
            terminal_writeline(" process(es); shell context restored.");
        }
    } else if (strings_equal(command, "stop")) {
        terminal_writeline("No process runs while the shell owns the CPU.");
    } else if (strings_equal(command, "stats")) {
        print_timing_records();
    } else {
        terminal_write("Unknown command: ");
        terminal_writeline(command);
        terminal_writeline("Type 'help' for available commands.");
    }
}

void shell_run(void)
{
    char input[SHELL_INPUT_CAPACITY];
    unsigned int length = 0;

    shell_banner();
    telemetry_publish();
    terminal_write("MiniOS> ");

    for (;;) {
        char character = keyboard_read_char();

        if (character == '\n') {
            terminal_putchar('\n');
            input[length] = '\0';
            shell_execute(input);
            telemetry_publish();
            length = 0;
            terminal_putchar('\n');
            terminal_write("MiniOS> ");
        } else if (character == '\b') {
            if (length != 0) {
                --length;
                terminal_backspace();
            }
        } else if (character >= ' ' && character <= '~' &&
                   length + 1 < SHELL_INPUT_CAPACITY) {
            input[length] = character;
            ++length;
            terminal_putchar(character);
        }
    }
}
