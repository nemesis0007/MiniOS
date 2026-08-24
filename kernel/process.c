#include "process.h"

static process_control_block_t process_table[PROCESS_MAX_COUNT];
static unsigned char process_stacks[PROCESS_MAX_COUNT][PROCESS_STACK_SIZE]
    __attribute__((aligned(16)));
static unsigned int next_pid;
static unsigned int used_slots;

static void clear_bytes(void *destination, unsigned int size)
{
    unsigned char *bytes = (unsigned char *)destination;
    unsigned int index;

    for (index = 0; index < size; ++index) {
        bytes[index] = 0;
    }
}

static void copy_name(char *destination, const char *source)
{
    unsigned int index = 0;

    while (source[index] != '\0' && index + 1 < PROCESS_NAME_CAPACITY) {
        destination[index] = source[index];
        ++index;
    }
    destination[index] = '\0';
}

static int names_equal(const char *left, const char *right)
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

void process_initialize(void)
{
    clear_bytes(process_table, sizeof(process_table));
    next_pid = 1;
    used_slots = 0;
}

process_control_block_t *process_create(const char *name,
                                          unsigned int priority,
                                          unsigned int arrival_time,
                                          unsigned int burst_time,
                                          process_entry_t entry)
{
    process_control_block_t *process;
    unsigned int stack_top;

    if (name == 0 || name[0] == '\0' || burst_time == 0 ||
        used_slots == PROCESS_MAX_COUNT) {
        return 0;
    }

    process = &process_table[used_slots];
    clear_bytes(process, sizeof(*process));
    process->pid = next_pid;
    ++next_pid;
    copy_name(process->name, name);
    process->state = PROCESS_NEW;
    process->priority = priority;
    process->arrival_time = arrival_time;
    process->burst_time = burst_time;
    process->remaining_time = burst_time;
    process->program_counter = (unsigned int)entry;
    process->stack_bottom = (unsigned int)&process_stacks[used_slots][0];
    stack_top = (unsigned int)&process_stacks[used_slots][PROCESS_STACK_SIZE];
    stack_top &= ~0xFU;
    process->stack_pointer = stack_top;
    process->context.esp = stack_top;
    process->context.eip = process->program_counter;
    process->context.eflags = 0x202;
    ++used_slots;
    return process;
}

process_control_block_t *process_find(unsigned int pid)
{
    unsigned int index;

    for (index = 0; index < used_slots; ++index) {
        if (process_table[index].pid == pid) {
            return &process_table[index];
        }
    }
    return 0;
}

int process_admit(unsigned int pid)
{
    process_control_block_t *process = process_find(pid);

    if (process == 0 || process->state != PROCESS_NEW) {
        return 0;
    }
    process->state = PROCESS_READY;
    return 1;
}

int process_terminate(unsigned int pid)
{
    process_control_block_t *process = process_find(pid);

    if (process == 0 || process->state == PROCESS_TERMINATED) {
        return 0;
    }
    process->state = PROCESS_TERMINATED;
    process->remaining_time = 0;
    return 1;
}

int process_prepare_context(process_control_block_t *process,
                            process_entry_t return_handler)
{
    unsigned int *stack;

    if (process == 0 || return_handler == 0 ||
        process->context_initialized != 0) {
        return 0;
    }

    stack = (unsigned int *)process->stack_pointer;
    *--stack = (unsigned int)return_handler;
    *--stack = process->program_counter;
    *--stack = 0x202;
    *--stack = 0;
    *--stack = 0;
    *--stack = 0;
    *--stack = 0;
    process->stack_pointer = (unsigned int)stack;
    process->context.esp = process->stack_pointer;
    process->context_initialized = 1;
    return 1;
}

const process_control_block_t *process_at(unsigned int index)
{
    if (index >= used_slots) {
        return 0;
    }
    return &process_table[index];
}

unsigned int process_slot_count(void)
{
    return used_slots;
}

const char *process_state_name(process_state_t state)
{
    switch (state) {
    case PROCESS_NEW:
        return "NEW";
    case PROCESS_READY:
        return "READY";
    case PROCESS_RUNNING:
        return "RUNNING";
    case PROCESS_WAITING:
        return "WAITING";
    case PROCESS_TERMINATED:
        return "TERMINATED";
    default:
        return "UNUSED";
    }
}

static void self_test_entry(void)
{
}

int process_self_test(void)
{
    process_control_block_t *process;
    int passed;

    process_initialize();
    process = process_create("selftest", 3, 7, 11, self_test_entry);
    passed = process != 0 && process->pid == 1 &&
             names_equal(process->name, "selftest") &&
             process->state == PROCESS_NEW && process->priority == 3 &&
             process->arrival_time == 7 && process->burst_time == 11 &&
             process->remaining_time == 11 && process->stack_pointer != 0 &&
             process_admit(process->pid) != 0 &&
             process->state == PROCESS_READY &&
             process_terminate(process->pid) != 0 &&
             process->state == PROCESS_TERMINATED;
    process_initialize();
    return passed;
}
