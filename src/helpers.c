#include "helpers.h"

#include <string.h>
#include <stdio.h>

#define MAX_TIMERS 10
#define PRETTY_FORMAT_BUF_SIZE 30

static struct timeval s_start_times[MAX_TIMERS];
static char s_pretty_format_buf[PRETTY_FORMAT_BUF_SIZE];

static void _init(void);

TimerID start_time_measure(void) {
    _init();
    TimerID available_timer = 0;
    while (available_timer < MAX_TIMERS)
    {
        // not in use, let's take it
        if (s_start_times[available_timer].tv_sec == 0)
            break;

        ++available_timer;
    }

    if (available_timer >= MAX_TIMERS)
        return INVALID_TIMER_ID;

    gettimeofday(&s_start_times[available_timer], NULL);
    return available_timer;
}

struct timeval end_time_measure(TimerID timer_id) {
    if (timer_id > MAX_TIMERS || s_start_times[timer_id].tv_sec == 0)
        return (struct timeval) { .tv_sec = 0, .tv_usec = 0 };

    struct timeval* start_time = &s_start_times[timer_id];
    struct timeval current_time;
    gettimeofday(&current_time, NULL);

    const struct timeval time_diff = {
        .tv_sec = current_time.tv_sec - start_time->tv_sec,
        .tv_usec = current_time.tv_usec - start_time->tv_usec,
    };
    // zero it out to denote we're done with it
    memset(start_time, 0, sizeof(s_start_times[0]));

    return time_diff;
}

char* pretty_format_time_diff(const struct timeval time_diff) {
    const time_t sec = time_diff.tv_sec;
    const time_t ms = time_diff.tv_usec / 1000;

    memset(s_pretty_format_buf, '\0', sizeof(s_pretty_format_buf[0]*PRETTY_FORMAT_BUF_SIZE));
    snprintf(s_pretty_format_buf, PRETTY_FORMAT_BUF_SIZE-1, "%lds %ldms", sec, ms);

    return s_pretty_format_buf;
}

static void _init(void) {
    static int initialised = 0;
    if (initialised) return;

    memset(s_start_times, 0, sizeof(s_start_times[0])*MAX_TIMERS);
}
