#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>
#include <sys/time.h>

typedef uint32_t TimerID;
typedef void (*TimerCallback)(void);

#define INVALID_TIMER_ID 0xFFFFFFFF

TimerID start_time_measure(void);
struct timeval end_time_measure(TimerID* timer_id);
char* pretty_format_time_diff(const struct timeval time_diff);

TimerID start_timer(TimerCallback callback, uint64_t duration_ms);
void update_timer(TimerID* timer_id);

#endif // #ifndef HELPERS_H

