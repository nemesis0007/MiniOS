#ifndef MINIOS_EXPERIMENT_H
#define MINIOS_EXPERIMENT_H

#include "statistics.h"

typedef struct experiment_result {
    const char *algorithm;
    system_statistics_t statistics;
} experiment_result_t;

int experiment_run(char workload_name);
unsigned int experiment_result_count(void);
const experiment_result_t *experiment_result_at(unsigned int index);
const char *experiment_workload_description(char workload_name);

#endif

