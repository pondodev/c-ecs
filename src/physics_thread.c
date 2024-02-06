#include "physics_thread.h"

#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>

#include "systems.h"

#define PHYSICS_STEPS_PER_SECOND 60
#define MS_PER_PHYSICS_STEP 1000 / PHYSICS_STEPS_PER_SECOND

static pthread_t s_thread;
static atomic_int s_app_running = 1;

static void* _physics_thread(void* args);
static uint64_t _get_unix_timestamp_ms(void);

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

    uint64_t last_step = _get_unix_timestamp_ms();

    while (s_app_running) {
        const uint64_t step_start = _get_unix_timestamp_ms();
        const float step_diff_ms = step_start - last_step;

        const float delta_time = step_diff_ms / 1000.f;
        system_physics(delta_time);

        last_step = step_start;

        const uint64_t now = _get_unix_timestamp_ms();
        const uint64_t diff = now - step_start;
        // TODO: something is wrong with this calculation
        const uint64_t ms_to_sleep = MS_PER_PHYSICS_STEP - diff;
        //usleep(ms_to_sleep * 1000);
        usleep(MS_PER_PHYSICS_STEP * 1000);
    }

    return NULL;
}

static uint64_t _get_unix_timestamp_ms(void) {
    struct timeval t;
    gettimeofday(&t, NULL);

    const uint64_t secs = t.tv_sec;
    const uint64_t msecs = t.tv_usec / 1000;

    return (secs*1000) + msecs;
}

