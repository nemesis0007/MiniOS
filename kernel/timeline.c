#include "timeline.h"

static timeline_event_t events[TIMELINE_MAX_EVENTS];
static unsigned int event_count;
static int overflow;

void timeline_initialize(void)
{
    event_count = 0;
    overflow = 0;
}

void timeline_record(unsigned int pid,
                     unsigned int start_time,
                     unsigned int end_time)
{
    timeline_event_t *previous;

    if (end_time <= start_time) {
        return;
    }
    if (event_count != 0) {
        previous = &events[event_count - 1];
        if (previous->pid == pid && previous->end_time == start_time) {
            previous->end_time = end_time;
            return;
        }
    }
    if (event_count == TIMELINE_MAX_EVENTS) {
        overflow = 1;
        return;
    }
    events[event_count].pid = pid;
    events[event_count].start_time = start_time;
    events[event_count].end_time = end_time;
    ++event_count;
}

unsigned int timeline_count(void)
{
    return event_count;
}

const timeline_event_t *timeline_at(unsigned int index)
{
    if (index >= event_count) {
        return 0;
    }
    return &events[index];
}

int timeline_overflowed(void)
{
    return overflow;
}

