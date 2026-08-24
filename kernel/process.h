#ifndef MINIOS_PROCESS_H
#define MINIOS_PROCESS_H

enum {
    PROCESS_MAX_COUNT = 16,
    PROCESS_NAME_CAPACITY = 16,
    PROCESS_STACK_SIZE = 4096
};

typedef enum process_state {
    PROCESS_UNUSED,
    PROCESS_NEW,
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_WAITING,
    PROCESS_TERMINATED
} process_state_t;

typedef struct cpu_context {
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
    unsigned int esi;
    unsigned int edi;
    unsigned int ebp;
    unsigned int esp;
    unsigned int eip;
    unsigned int eflags;
} cpu_context_t;

typedef struct process_statistics {
    unsigned int start_time;
    unsigned int completion_time;
    unsigned int waiting_time;
    unsigned int turnaround_time;
    unsigned int response_time;
    unsigned int has_started;
} process_statistics_t;

typedef struct process_control_block {
    unsigned int pid;
    char name[PROCESS_NAME_CAPACITY];
    process_state_t state;
    unsigned int priority;
    unsigned int arrival_time;
    unsigned int burst_time;
    unsigned int remaining_time;
    unsigned int wake_time;
    unsigned int blocked_time;
    unsigned int program_counter;
    unsigned int stack_bottom;
    unsigned int stack_pointer;
    unsigned int context_initialized;
    cpu_context_t context;
    process_statistics_t statistics;
} process_control_block_t;

typedef void (*process_entry_t)(void);

void process_initialize(void);
process_control_block_t *process_create(const char *name,
                                          unsigned int priority,
                                          unsigned int arrival_time,
                                          unsigned int burst_time,
                                          process_entry_t entry);
int process_admit(unsigned int pid);
int process_terminate(unsigned int pid);
int process_prepare_context(process_control_block_t *process,
                            process_entry_t return_handler);
process_control_block_t *process_find(unsigned int pid);
const process_control_block_t *process_at(unsigned int index);
unsigned int process_slot_count(void);
const char *process_state_name(process_state_t state);
int process_self_test(void);

#endif
