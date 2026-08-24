#ifndef MINIOS_SCHEDULER_H
#define MINIOS_SCHEDULER_H

#include "process.h"

typedef enum scheduler_algorithm {
    SCHEDULER_FCFS,
    SCHEDULER_SJF,
    SCHEDULER_PRIORITY,
    SCHEDULER_ROUND_ROBIN
} scheduler_algorithm_t;

void scheduler_initialize(void);
int scheduler_select_name(const char *name);
const char *scheduler_name(void);
void scheduler_set_quantum(unsigned int ticks);
unsigned int scheduler_quantum(void);
unsigned int scheduler_start(void);
unsigned int scheduler_clock(void);
unsigned int scheduler_busy_ticks(void);
process_control_block_t *scheduler_on_timer_tick(void);
process_control_block_t *scheduler_process_finished(
    process_control_block_t *finished);
int scheduler_current_has_work(void);
void scheduler_sleep_current(unsigned int ticks);
unsigned int scheduler_context_switches(void);

#endif
