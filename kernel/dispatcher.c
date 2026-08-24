#include "dispatcher.h"
#include "context_switch.h"
#include "interrupt.h"
#include "process.h"
#include "scheduler.h"

static unsigned int kernel_stack_pointer;
static process_control_block_t *current_process;

static int prepare_process(process_control_block_t *process)
{
    if (process->context_initialized == 0) {
        return process_prepare_context(process, dispatcher_process_exit);
    }
    return 1;
}

unsigned int dispatcher_run(process_control_block_t *process)
{
    unsigned int pid;

    if (process == 0 ||
        (process->state != PROCESS_READY &&
         process->state != PROCESS_RUNNING)) {
        return 0;
    }

    pid = process->pid;
    if (prepare_process(process) == 0) {
        return 0;
    }
    current_process = process;
    process->state = PROCESS_RUNNING;
    context_switch(&kernel_stack_pointer, process->stack_pointer);
    current_process = 0;
    return pid;
}

void dispatcher_preempt(process_control_block_t *next_process)
{
    process_control_block_t *previous = current_process;

    if (previous == 0 || next_process == 0 || previous == next_process ||
        prepare_process(next_process) == 0) {
        return;
    }

    current_process = next_process;
    context_switch(&previous->stack_pointer, next_process->stack_pointer);
}

void dispatcher_yield(process_control_block_t *next_process)
{
    process_control_block_t *previous = current_process;

    if (previous == 0 || previous == next_process) {
        return;
    }
    if (next_process != 0) {
        if (prepare_process(next_process) == 0) {
            return;
        }
        current_process = next_process;
        context_switch(&previous->stack_pointer, next_process->stack_pointer);
    } else {
        current_process = 0;
        context_switch(&previous->stack_pointer, kernel_stack_pointer);
    }
}

process_control_block_t *dispatcher_current(void)
{
    return current_process;
}

void dispatcher_process_exit(void)
{
    process_control_block_t *finished;
    process_control_block_t *next;

    interrupt_disable();
    finished = current_process;
    if (finished != 0) {
        finished->remaining_time = 0;
        finished->state = PROCESS_TERMINATED;
        next = scheduler_process_finished(finished);
        if (next != 0 && prepare_process(next) != 0) {
            current_process = next;
            context_switch(&finished->stack_pointer, next->stack_pointer);
        }
        current_process = 0;
        context_switch(&finished->stack_pointer, kernel_stack_pointer);
    }

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
