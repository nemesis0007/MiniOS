#ifndef MINIOS_PROGRAMS_H
#define MINIOS_PROGRAMS_H

#include "process.h"
#include "scheduler.h"

typedef struct program_descriptor {
    const char *name;
    process_entry_t entry;
    unsigned int estimated_burst;
    unsigned int default_priority;
} program_descriptor_t;

const program_descriptor_t *program_lookup(const char *name);
const program_descriptor_t *program_at(unsigned int index);
unsigned int program_count(void);
int program_registry_self_test(void);

void program_counter(void);
void program_fibonacci(void);
void program_prime(void);
void program_short_task(void);
void program_long_task(void);
void program_io_test(void);

static inline int program_has_cpu_work(void)
{
    return scheduler_current_has_work();
}

#endif
