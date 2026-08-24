#ifndef MINIOS_TIMELINE_H
#define MINIOS_TIMELINE_H

enum { TIMELINE_MAX_EVENTS = 128 };

typedef struct timeline_event {
    unsigned int pid;
    unsigned int start_time;
    unsigned int end_time;
} timeline_event_t;

void timeline_initialize(void);
void timeline_record(unsigned int pid,
                     unsigned int start_time,
                     unsigned int end_time);
unsigned int timeline_count(void);
const timeline_event_t *timeline_at(unsigned int index);
int timeline_overflowed(void);

#endif

