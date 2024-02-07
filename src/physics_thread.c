#include "physics_thread.h"

#include <stdio.h>
#include <pthread.h>
#include <pthread/sched.h>
#include <stdatomic.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>

#include "systems.h"
#include "ecs.h"

#define PHYSICS_STEPS_PER_SECOND 60
#define MS_PER_PHYSICS_STEP 1000 / PHYSICS_STEPS_PER_SECOND
#define US_PER_PHYSICS_STEP MS_PER_PHYSICS_STEP * 1000

static pthread_t s_thread;
static atomic_int s_app_running = 1;

static void* _physics_thread(void* args);
static uint64_t _timeval_to_timestamp_ms(struct timeval tv);
static uint32_t _get_diff_us(struct timeval start, struct timeval end);

int start_physics_thread(void) {
    const int err = pthread_create(&s_thread, NULL, _physics_thread, NULL);
    if (err != 0) {
        fprintf(stderr, "ERROR: failed to start physics thread (%s)\n", strerror(err));
        return 0;
    }

    return 1;
}

void join_physics_thread(void) {
    s_app_running = 0;
    const int err = pthread_join(s_thread, NULL);
    if (err != 0)
        fprintf(stderr, "ERROR: failed to join physics thread (%s)\n", strerror(err));
}

static void* _physics_thread(void* args) {
    (void)args;

    struct timeval thread_start;
    gettimeofday(&thread_start, NULL);
    uint64_t last_step_ms = _timeval_to_timestamp_ms(thread_start);

    while (s_app_running) {
        ecs_lock_mutex();

        struct timeval step_start;
        gettimeofday(&step_start, NULL);

        const uint64_t step_start_ms = _timeval_to_timestamp_ms(step_start);
        const float step_diff_ms = step_start_ms - last_step_ms;

        const float delta_time = step_diff_ms / 1000.f;
        system_physics(delta_time);

        last_step_ms = step_start_ms;

        ecs_unlock_mutex();
        sched_yield();

        struct timeval step_end;
        gettimeofday(&step_end, NULL);

        const uint32_t time_taken_us = _get_diff_us(step_start, step_end);
        const uint32_t us_to_sleep = US_PER_PHYSICS_STEP - time_taken_us;

        usleep(us_to_sleep);
    }

    return NULL;
}

static uint64_t _timeval_to_timestamp_ms(struct timeval tv) {
    const uint64_t secs = tv.tv_sec;
    const uint64_t msecs = tv.tv_usec / 1000;

    return (secs * 1000) + msecs;
}

static uint32_t _get_diff_us(struct timeval start, struct timeval end) {
    const uint32_t start_ms = start.tv_sec / 1000;
    const uint32_t end_ms   = end.tv_sec / 1000;
    const uint32_t start_us = start.tv_usec;
    const uint32_t end_us   = start.tv_usec;

    uint32_t diff_us = 0;
    if (end_us < start_us) {
        const uint32_t overflow = start_us - end_us;
        diff_us = 1000 - overflow;
    } else {
        diff_us = end_us - start_us;
    }

    diff_us += (end_ms - start_ms) * 1000;
    return diff_us;
}

