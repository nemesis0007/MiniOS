#ifndef MINIOS_DISPATCHER_H
#define MINIOS_DISPATCHER_H

#include "process.h"

unsigned int dispatcher_run(process_control_block_t *process);
void dispatcher_preempt(process_control_block_t *next_process);
void dispatcher_yield(process_control_block_t *next_process);
process_control_block_t *dispatcher_current(void);
void dispatcher_process_exit(void) __attribute__((noreturn));

#endif
