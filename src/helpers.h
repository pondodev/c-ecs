#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>
#include <sys/time.h>

typedef uint32_t TimerID;
#define INVALID_TIMER_ID 0xFFFFFFFF

TimerID start_time_measure(void);
struct timeval end_time_measure(TimerID timer_id);
char* pretty_format_time_diff(const struct timeval time_diff);

#endif // #ifndef HELPERS_H

