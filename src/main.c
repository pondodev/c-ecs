#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"
#include "ecs.h"
#include "systems.h"
#include "helpers.h"

#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 512
#define MAX_COMPONENTS 100

static EntityID s_entities[MAX_COMPONENTS];

static int _irand_range(int min, int max) {
    static int rand_init = 0;
    if (! rand_init) {
        srand(time(NULL));
        rand_init = 1;
    }

    const int minmax_diff = max - min;
    return (rand() % minmax_diff) + min;
}

int main(void) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "c ecs");
    ecs_init(MAX_COMPONENTS);

    // init all entities
    for (size_t i = 0; i < MAX_COMPONENTS; ++i) {
        EntityID id = ecs_new_entity();
        s_entities[i] = id;
        PositionComponent* pos          = ecs_new_position_component(id);
        DisplayComponent* disp          = ecs_new_display_component(id);
        RigidBodyComponent* rb          = ecs_new_rigid_body_component(id);
        CircleColliderComponent* col    = ecs_new_circle_collider_component(id);

        pos->x = _irand_range(0, WINDOW_WIDTH);
        pos->y = _irand_range(0, WINDOW_HEIGHT);
        disp->color = (Color) {
            .r = _irand_range(0, 255),
            .g = _irand_range(0, 255),
            .b = _irand_range(0, 255),
            .a = 255,
        };
        disp->radius = _irand_range(3, 15);

        // move entities within the bounds of the screen
        if (pos->x - disp->radius < 0)
            pos->x = disp->radius;
        else if (pos->x + disp->radius > WINDOW_WIDTH)
            pos->x = WINDOW_WIDTH - disp->radius;

        if (pos->y - disp->radius < 0)
            pos->y = disp->radius;
        else if (pos->y + disp->radius > WINDOW_HEIGHT)
            pos->y = WINDOW_HEIGHT - disp->radius;
    }

    while (! WindowShouldClose())
    {
        // update state
        // TODO

        // drawing
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            system_draw();

            DrawFPS(0, 0);
        }
        EndDrawing();
    }

    CloseWindow();
    ecs_free();

    return 0;
}
