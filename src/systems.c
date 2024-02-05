#include "systems.h"

#include "ecs.h"
#include "raylib.h"

void system_draw(void) {
    DisplayComponent* displays = NULL;
    size_t display_count = 0;
    ecs_get_display_component_array(&displays, &display_count);
    if (displays == NULL || display_count == 0)
        return;

    for (size_t i = 0; i < display_count; ++i) {
        const DisplayComponent* disp = &displays[i];
        if (disp == NULL)
            continue;

        const EntityID id = disp->owner;
        const PositionComponent* pos = ecs_get_position_component(id);
        if (pos == NULL)
            continue;

        DrawCircle(pos->x, pos->y, disp->radius, disp->color);
    }
}

void system_physics(const float delta_time) {
    RigidBodyComponent* rigid_bodies = NULL;
    size_t rigid_body_count = 0;
    ecs_get_rigid_body_component_array(&rigid_bodies, &rigid_body_count);
    if (rigid_bodies == NULL || rigid_body_count == 0)
        return;

    const int screen_width = GetScreenWidth();
    const int screen_height = GetScreenHeight();

    for (size_t i = 0; i < rigid_body_count; ++i) {
        const RigidBodyComponent* rb = &rigid_bodies[i];
        if (rb == NULL)
            continue;

        const EntityID id = rb->owner;
        PositionComponent* pos = ecs_get_position_component(id);
        if (pos == NULL)
            continue;

        pos->x += rb->vel_x * delta_time;
        pos->y += rb->vel_y * delta_time;

        // wrap around
        // TODO: actual physics will bounce balls off edge of screen, so we will remove this
        if (pos->x < 0)
            pos->x = screen_width + pos->x;
        else if (pos->x > screen_width)
            pos->x = pos->x - screen_width;

        if (pos->y < 0)
            pos->y = screen_height + pos->y;
        else if (pos->y > screen_height)
            pos->y = pos->y - screen_height;
    }
}
