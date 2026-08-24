#include "scheduler.h"
#include "dispatcher.h"
#include "interrupt.h"
#include "timeline.h"

static scheduler_algorithm_t selected_algorithm;
static unsigned int logical_clock;
static unsigned int quantum_ticks;
static unsigned int quantum_used;
static unsigned int completed_count;
static unsigned int context_switch_count;
static unsigned int busy_ticks;
static int scheduler_active;

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

void scheduler_initialize(void)
{
    selected_algorithm = SCHEDULER_FCFS;
    logical_clock = 0;
    quantum_ticks = 2;
    quantum_used = 0;
    completed_count = 0;
    context_switch_count = 0;
    busy_ticks = 0;
    scheduler_active = 0;
    timeline_initialize();
}

int scheduler_select_name(const char *name)
{
    if (strings_equal(name, "fcfs")) {
        selected_algorithm = SCHEDULER_FCFS;
        return 1;
    }
    if (strings_equal(name, "rr")) {
        selected_algorithm = SCHEDULER_ROUND_ROBIN;
        return 1;
    }
    if (strings_equal(name, "sjf")) {
        selected_algorithm = SCHEDULER_SJF;
        return 1;
    }
    if (strings_equal(name, "priority")) {
        selected_algorithm = SCHEDULER_PRIORITY;
        return 1;
    }
    return 0;
}

const char *scheduler_name(void)
{
    if (selected_algorithm == SCHEDULER_ROUND_ROBIN) {
        return "Round Robin";
    }
    if (selected_algorithm == SCHEDULER_SJF) {
        return "SJF";
    }
    if (selected_algorithm == SCHEDULER_PRIORITY) {
        return "Priority";
    }
    return "FCFS";
}

void scheduler_set_quantum(unsigned int ticks)
{
    if (ticks != 0) {
        quantum_ticks = ticks;
    }
}

unsigned int scheduler_quantum(void)
{
    return quantum_ticks;
}

static int is_available(const process_control_block_t *process)
{
    return process->state == PROCESS_READY &&
           process->arrival_time <= logical_clock;
}

static process_control_block_t *select_fcfs(void)
{
    process_control_block_t *best = 0;
    unsigned int index;

    for (index = 0; index < process_slot_count(); ++index) {
        process_control_block_t *candidate =
            process_find(process_at(index)->pid);

        if (!is_available(candidate)) {
            continue;
        }
        if (best == 0 || candidate->arrival_time < best->arrival_time ||
            (candidate->arrival_time == best->arrival_time &&
             candidate->pid < best->pid)) {
            best = candidate;
        }
    }
    return best;
}

static process_control_block_t *select_sjf(void)
{
    process_control_block_t *best = 0;
    unsigned int index;

    for (index = 0; index < process_slot_count(); ++index) {
        process_control_block_t *candidate =
            process_find(process_at(index)->pid);

        if (!is_available(candidate)) {
            continue;
        }
        if (best == 0 || candidate->burst_time < best->burst_time ||
            (candidate->burst_time == best->burst_time &&
             candidate->pid < best->pid)) {
            best = candidate;
        }
    }
    return best;
}

static process_control_block_t *select_priority(void)
{
    process_control_block_t *best = 0;
    unsigned int index;

    for (index = 0; index < process_slot_count(); ++index) {
        process_control_block_t *candidate =
            process_find(process_at(index)->pid);

        if (!is_available(candidate)) {
            continue;
        }
        if (best == 0 || candidate->priority < best->priority ||
            (candidate->priority == best->priority &&
             candidate->pid < best->pid)) {
            best = candidate;
        }
    }
    return best;
}

static process_control_block_t *select_round_robin(unsigned int after_pid)
{
    unsigned int offset;
    unsigned int start = 0;

    if (process_slot_count() == 0) {
        return 0;
    }
    for (offset = 0; offset < process_slot_count(); ++offset) {
        if (process_at(offset)->pid == after_pid) {
            start = (offset + 1) % process_slot_count();
            break;
        }
    }
    for (offset = 0; offset < process_slot_count(); ++offset) {
        unsigned int index = (start + offset) % process_slot_count();
        process_control_block_t *candidate =
            process_find(process_at(index)->pid);

        if (is_available(candidate)) {
            return candidate;
        }
    }
    return 0;
}

static process_control_block_t *select_next(unsigned int after_pid)
{
    if (selected_algorithm == SCHEDULER_ROUND_ROBIN) {
        return select_round_robin(after_pid);
    }
    if (selected_algorithm == SCHEDULER_SJF) {
        return select_sjf();
    }
    if (selected_algorithm == SCHEDULER_PRIORITY) {
        return select_priority();
    }
    return select_fcfs();
}

static void wake_waiting_processes(void)
{
    unsigned int index;

    for (index = 0; index < process_slot_count(); ++index) {
        process_control_block_t *process =
            process_find(process_at(index)->pid);

        if (process->state == PROCESS_WAITING &&
            process->wake_time <= logical_clock) {
            process->state = PROCESS_READY;
        }
    }
}

static void begin_timeslice(process_control_block_t *process)
{
    if (process->statistics.has_started == 0) {
        process->statistics.start_time = logical_clock;
        process->statistics.response_time =
            logical_clock - process->arrival_time;
        process->statistics.has_started = 1;
    }
    process->state = PROCESS_RUNNING;
    quantum_used = 0;
    ++context_switch_count;
}

static void advance_to_next_arrival(void)
{
    unsigned int index;
    unsigned int earliest = 0xFFFFFFFFU;

    for (index = 0; index < process_slot_count(); ++index) {
        const process_control_block_t *process = process_at(index);

        unsigned int ready_time;

        if (process->state == PROCESS_READY) {
            ready_time = process->arrival_time;
        } else if (process->state == PROCESS_WAITING) {
            ready_time = process->wake_time;
        } else {
            continue;
        }
        if (ready_time < earliest) {
            earliest = ready_time;
        }
    }
    if (earliest != 0xFFFFFFFFU && logical_clock < earliest) {
        unsigned int elapsed = earliest - logical_clock;

        for (index = 0; index < process_slot_count(); ++index) {
            process_control_block_t *process =
                process_find(process_at(index)->pid);

            if (process->state == PROCESS_WAITING) {
                process->blocked_time += elapsed;
            }
        }
        logical_clock = earliest;
    }
    wake_waiting_processes();
}

unsigned int scheduler_start(void)
{
    unsigned int before = completed_count;
    process_control_block_t *process;

    scheduler_active = 1;
    advance_to_next_arrival();

    if (selected_algorithm != SCHEDULER_ROUND_ROBIN) {
        while ((process = select_next(0)) != 0) {
            if (logical_clock < process->arrival_time) {
                logical_clock = process->arrival_time;
            }
            begin_timeslice(process);
            if (dispatcher_run(process) == 0) {
                break;
            }
            advance_to_next_arrival();
        }
        scheduler_active = 0;
    } else {
        for (;;) {
            advance_to_next_arrival();
            process = select_round_robin(0);
            if (process == 0) {
                break;
            }
            begin_timeslice(process);
            dispatcher_run(process);
        }
        scheduler_active = 0;
    }
    return completed_count - before;
}

process_control_block_t *scheduler_on_timer_tick(void)
{
    process_control_block_t *current = dispatcher_current();
    process_control_block_t *next;

    if (scheduler_active == 0 || current == 0 ||
        current->state != PROCESS_RUNNING) {
        return 0;
    }

    timeline_record(current->pid, logical_clock, logical_clock + 1);
    ++logical_clock;
    ++busy_ticks;
    ++quantum_used;
    if (current->remaining_time != 0) {
        --current->remaining_time;
    }
    {
        unsigned int index;

        for (index = 0; index < process_slot_count(); ++index) {
            process_control_block_t *process =
                process_find(process_at(index)->pid);

            if (process->state == PROCESS_WAITING) {
                ++process->blocked_time;
            }
        }
    }
    wake_waiting_processes();

    if (selected_algorithm != SCHEDULER_ROUND_ROBIN ||
        current->remaining_time == 0 || quantum_used < quantum_ticks) {
        return 0;
    }

    current->state = PROCESS_READY;
    next = select_round_robin(current->pid);
    if (next == 0 || next == current) {
        current->state = PROCESS_RUNNING;
        quantum_used = 0;
        return 0;
    }

    begin_timeslice(next);
    return next;
}

process_control_block_t *scheduler_process_finished(
    process_control_block_t *finished)
{
    process_control_block_t *next;

    finished->statistics.completion_time = logical_clock;
    finished->statistics.turnaround_time =
        logical_clock - finished->arrival_time;
    if (finished->statistics.turnaround_time >= finished->burst_time) {
        finished->statistics.waiting_time =
            finished->statistics.turnaround_time - finished->burst_time;
        if (finished->statistics.waiting_time >= finished->blocked_time) {
            finished->statistics.waiting_time -= finished->blocked_time;
        }
    }
    ++completed_count;

    if (selected_algorithm != SCHEDULER_ROUND_ROBIN ||
        scheduler_active == 0) {
        return 0;
    }

    wake_waiting_processes();
    next = select_round_robin(finished->pid);
    if (next == 0) {
        return 0;
    }
    begin_timeslice(next);
    return next;
}

int scheduler_current_has_work(void)
{
    process_control_block_t *current = dispatcher_current();
    return current != 0 && current->remaining_time != 0;
}

void scheduler_sleep_current(unsigned int ticks)
{
    process_control_block_t *current;
    process_control_block_t *next;

    if (ticks == 0) {
        return;
    }
    interrupt_disable();
    current = dispatcher_current();
    if (current == 0 || current->state != PROCESS_RUNNING) {
        interrupt_enable();
        return;
    }
    current->state = PROCESS_WAITING;
    current->wake_time = logical_clock + ticks;
    next = select_next(current->pid);
    if (next != 0) {
        begin_timeslice(next);
    }
    dispatcher_yield(next);
    interrupt_enable();
}

unsigned int scheduler_clock(void)
{
    return logical_clock;
}

unsigned int scheduler_busy_ticks(void)
{
    return busy_ticks;
}

unsigned int scheduler_context_switches(void)
{
    return context_switch_count;
}
