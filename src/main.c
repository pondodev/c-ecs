#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <pthread/sched.h>

#include "raylib.h"
#include "ecs.h"
#include "systems.h"
#include "physics_thread.h"
#include "helpers.h"

#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 512

#define MAX_ENTITY_COUNT 1000
#define START_ENTITY_COUNT 10
#define MAX_COMPONENTS 100

static EntityID s_entities[MAX_ENTITY_COUNT];
static int s_running = 1;

static void _init_entities(void);
static void _rand_init(void);
static int _irand_range(int min, int max);
static float _frand_range(float min, float max);

int main(void) {
    assert(ENTITY_COUNT <= MAX_COMPONENTS);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "c ecs");
    ecs_init(MAX_COMPONENTS);
    _init_entities();

    if (! start_physics_thread())
        s_running = 0;

    while (! WindowShouldClose() && s_running) {
        // drawing
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            ecs_lock_mutex();

            system_draw();

            ecs_unlock_mutex();
            sched_yield();

            DrawFPS(0, 0);
        }
        EndDrawing();
    }

    join_physics_thread();

    CloseWindow();
    ecs_free();

    return 0;
}

static void _init_entities(void) {
    for (size_t i = 0; i < START_ENTITY_COUNT; ++i) {
        EntityID id = ecs_new_entity();
        s_entities[i] = id;
        PositionComponent* pos          = ecs_new_position_component(id);
        DisplayComponent* disp          = ecs_new_display_component(id);
        RigidBodyComponent* rb          = ecs_new_rigid_body_component(id);
        CircleColliderComponent* col    = ecs_new_circle_collider_component(id);

        pos->pos.x = _irand_range(0, WINDOW_WIDTH);
        pos->pos.y = _irand_range(0, WINDOW_HEIGHT);
        disp->color = (Color) {
            .r = _irand_range(0, 255),
            .g = _irand_range(0, 255),
            .b = _irand_range(0, 255),
            .a = 255,
        };

        disp->radius = _irand_range(3, 15);

        col->radius = disp->radius;

        rb->mass = col->radius / 10.f;
        rb->velocity.x = _frand_range(-50.f, 50.f);
        rb->velocity.y = _frand_range(-50.f, 50.f);

        // move entities within the bounds of the screen
        if (pos->pos.x - disp->radius < 0)
            pos->pos.x = disp->radius;
        else if (pos->pos.x + disp->radius > WINDOW_WIDTH)
            pos->pos.x = WINDOW_WIDTH - disp->radius;

        if (pos->pos.y - disp->radius < 0)
            pos->pos.y = disp->radius;
        else if (pos->pos.y + disp->radius > WINDOW_HEIGHT)
            pos->pos.y = WINDOW_HEIGHT - disp->radius;
    }
}

static void _rand_init(void) {
    static int rand_init = 0;
    if (! rand_init) {
        srand(time(NULL));
        rand_init = 1;
    }
}

static int _irand_range(int min, int max) {
    _rand_init();

    const int minmax_diff = max - min;
    return (rand() % minmax_diff) + min;
}

static float _frand_range(float min, float max) {
    _rand_init();

    const float num = sin(rand() * rand());
    return min + (max - min) * fabs(num);
}

