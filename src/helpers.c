#include "helpers.h"

#include <string.h>
#include <stdio.h>

typedef struct {
    TimerCallback   callback;
    void*           args;
} CallbackInfo;

#define MAX_TIMERS 50
#define PRETTY_FORMAT_BUF_SIZE 30

static int s_timer_in_use[MAX_TIMERS];
static struct timeval s_start_times[MAX_TIMERS];
static char s_pretty_format_buf[PRETTY_FORMAT_BUF_SIZE];

static CallbackInfo s_timer_callbacks[MAX_TIMERS];
static uint64_t s_timer_durations[MAX_TIMERS];

static void _init(void);
static TimerID _get_free_timer(void);
static void _free_timer(TimerID* timer_id);
static uint64_t _timeval_to_ms(struct timeval tv);

TimerID start_time_measure(void) {
    _init();

    TimerID available_timer = _get_free_timer();
    if (available_timer != INVALID_TIMER_ID)
        gettimeofday(&s_start_times[available_timer], NULL);

    return available_timer;
}

struct timeval end_time_measure(TimerID* timer_id) {
    if (*timer_id > MAX_TIMERS || s_start_times[*timer_id].tv_sec == 0)
        return (struct timeval) { .tv_sec = 0, .tv_usec = 0 };

    struct timeval* start_time = &s_start_times[*timer_id];
    struct timeval current_time;
    gettimeofday(&current_time, NULL);

    const struct timeval time_diff = {
        .tv_sec = current_time.tv_sec - start_time->tv_sec,
        .tv_usec = current_time.tv_usec - start_time->tv_usec,
    };

    _free_timer(timer_id);

    return time_diff;
}

char* pretty_format_time_diff(const struct timeval time_diff) {
    const time_t sec = time_diff.tv_sec;
    const time_t ms = time_diff.tv_usec / 1000;

    memset(s_pretty_format_buf, '\0', sizeof(s_pretty_format_buf[0]*PRETTY_FORMAT_BUF_SIZE));
    snprintf(s_pretty_format_buf, PRETTY_FORMAT_BUF_SIZE-1, "%lds %ldms", sec, ms);

    return s_pretty_format_buf;
}

TimerID start_timer(TimerCallback callback, void* args, uint64_t duration_ms) {
    _init();

    if (callback == NULL)
        return INVALID_TIMER_ID;

    TimerID available_timer = _get_free_timer();
    if (available_timer != INVALID_TIMER_ID) {
        gettimeofday(&s_start_times[available_timer], NULL);
        s_timer_durations[available_timer] = duration_ms;

        s_timer_callbacks[available_timer] = (CallbackInfo) {
            .callback = callback,
            .args = args,
        };
    }

    return available_timer;
}

void update_timer(TimerID* timer_id) {
    if (*timer_id >= MAX_TIMERS)
        return;

    const struct timeval start_time     = s_start_times[*timer_id];
    const uint64_t timer_duration_ms    = s_timer_durations[*timer_id];
    const CallbackInfo callback_info    = s_timer_callbacks[*timer_id];

    if (callback_info.callback == NULL) {
        _free_timer(timer_id);
        return;
    }

    struct timeval now;
    gettimeofday(&now, NULL);

    const uint64_t start_ms = _timeval_to_ms(start_time);
    const uint64_t now_ms = _timeval_to_ms(now);
    const uint64_t diff = now_ms - start_ms;

    if (diff < timer_duration_ms)
        return;

    TimerCallback callback = callback_info.callback;
    void* args = callback_info.args;
    callback(args);

    _free_timer(timer_id);
}

static void _init(void) {
    static int initialised = 0;
    if (initialised) return;

    memset(s_timer_in_use, 0, sizeof(s_timer_in_use[0])*MAX_TIMERS);
    memset(s_start_times, 0, sizeof(s_start_times[0])*MAX_TIMERS);
    memset(s_timer_callbacks, 0, sizeof(s_timer_callbacks[0])*MAX_TIMERS);
    memset(s_timer_durations, 0, sizeof(s_timer_durations[0])*MAX_TIMERS);

    initialised = 1;
}

static TimerID _get_free_timer(void) {
    TimerID available_timer = 0;
    while (available_timer < MAX_TIMERS) {
        printf("checking if %d is free\n", available_timer);
        if (! s_timer_in_use[available_timer])
            break;

        ++available_timer;
    }

    if (available_timer >= MAX_TIMERS)
        return INVALID_TIMER_ID;

    s_timer_in_use[available_timer] = 1;
    return available_timer;
}

static void _free_timer(TimerID* timer_id) {
    s_timer_in_use[*timer_id] = 0;
    memset(&s_start_times[*timer_id], 0, sizeof(s_start_times[0]));
    memset(&s_timer_callbacks[*timer_id], 0, sizeof(s_timer_callbacks[0]));
    s_timer_durations[*timer_id] = 0;

    // invalidate the caller's ID so it can't be used again
    *timer_id = INVALID_TIMER_ID;
}

static uint64_t _timeval_to_ms(struct timeval tv) {
    return (tv.tv_sec*1000) + (tv.tv_usec/1000);
}

