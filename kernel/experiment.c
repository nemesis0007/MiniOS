#include "experiment.h"
#include "process.h"
#include "programs.h"
#include "scheduler.h"

enum { EXPERIMENT_ALGORITHM_COUNT = 4 };

static const char *const algorithms[EXPERIMENT_ALGORITHM_COUNT] = {
    "fcfs", "sjf", "priority", "rr"
};
static const char *const workload_a[] = {
    "counter", "fibonacci", "prime"
};
static const char *const workload_b[] = {
    "short", "long", "io_test", "fibonacci"
};
static experiment_result_t results[EXPERIMENT_ALGORITHM_COUNT];
static unsigned int result_count;

static int create_workload(const char *const *names, unsigned int count)
{
    unsigned int index;

    for (index = 0; index < count; ++index) {
        const program_descriptor_t *program = program_lookup(names[index]);
        process_control_block_t *process;

        if (program == 0) {
            return 0;
        }
        process = process_create(program->name,
                                 program->default_priority,
                                 0,
                                 program->estimated_burst,
                                 program->entry);
        if (process == 0 || process_admit(process->pid) == 0) {
            return 0;
        }
    }
    return 1;
}

int experiment_run(char workload_name)
{
    const char *const *workload;
    unsigned int workload_count;
    unsigned int algorithm_index;

    if (workload_name == 'a') {
        workload = workload_a;
        workload_count = sizeof(workload_a) / sizeof(workload_a[0]);
    } else if (workload_name == 'b') {
        workload = workload_b;
        workload_count = sizeof(workload_b) / sizeof(workload_b[0]);
    } else {
        return 0;
    }

    result_count = 0;
    for (algorithm_index = 0;
         algorithm_index < EXPERIMENT_ALGORITHM_COUNT;
         ++algorithm_index) {
        process_initialize();
        scheduler_initialize();
        scheduler_set_quantum(2);
        if (scheduler_select_name(algorithms[algorithm_index]) == 0 ||
            create_workload(workload, workload_count) == 0 ||
            scheduler_start() != workload_count) {
            result_count = 0;
            return 0;
        }
        results[result_count].algorithm = scheduler_name();
        results[result_count].statistics = statistics_calculate();
        ++result_count;
    }
    return 1;
}

unsigned int experiment_result_count(void)
{
    return result_count;
}

const experiment_result_t *experiment_result_at(unsigned int index)
{
    if (index >= result_count) {
        return 0;
    }
    return &results[index];
}

const char *experiment_workload_description(char workload_name)
{
    if (workload_name == 'a') {
        return "counter, fibonacci, prime";
    }
    if (workload_name == 'b') {
        return "short, long, io_test, fibonacci";
    }
    return "unknown";
}

