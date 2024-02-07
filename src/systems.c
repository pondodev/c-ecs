#include "systems.h"

#include "ecs.h"
#include "vec_maths.h"
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

        DrawCircle(pos->pos.x, pos->pos.y, disp->radius, disp->color);
    }
}

#define GRAVITY 2000.f
#define DRAG_COEFFICIENT 0.01f

void system_physics(const float delta_time) {
    RigidBodyComponent* rigid_bodies = NULL;
    size_t rigid_body_count = 0;
    ecs_get_rigid_body_component_array(&rigid_bodies, &rigid_body_count);

    if (rigid_bodies == NULL || rigid_body_count == 0)
        return;

    const float screen_width = GetScreenWidth();
    const float screen_height = GetScreenHeight();

    for (size_t i = 0; i < rigid_body_count; ++i) {
        RigidBodyComponent* rb = &rigid_bodies[i];
        if (rb == NULL)
            continue;

        const EntityID id = rb->owner;
        PositionComponent* pos = ecs_get_position_component(id);
        const CircleColliderComponent* col = ecs_get_circle_collider_component(id);

        if (pos == NULL || col == NULL)
            continue;

        // modify velocity based on gravity and drag
        rb->velocity.y += GRAVITY * rb->mass * delta_time;

        Vec2 drag = vec2_invert(rb->velocity);
        drag = vec2_mul(drag, DRAG_COEFFICIENT);

        rb->velocity = vec2_add(rb->velocity, drag);

        Vec2 new_pos = {
            .x = pos->pos.x + (rb->velocity.x * delta_time),
            .y = pos->pos.y + (rb->velocity.y * delta_time),
        };

        const Vec2 max_bound = {
            .x = screen_width - col->radius,
            .y = screen_height - col->radius,
        };
        const Vec2 min_bound = {
            .x = col->radius,
            .y = col->radius,
        };

        if (new_pos.x > max_bound.x) {
            rb->velocity.x *= -1;
            new_pos.x = max_bound.x;
        } else if (new_pos.x < min_bound.x) {
            rb->velocity.x *= -1;
            new_pos.x = min_bound.x;
        }

        if (new_pos.y > max_bound.y) {
            rb->velocity.y *= -1;
            new_pos.y = max_bound.y;
        } else if (new_pos.y < min_bound.y) {
            rb->velocity.y *= -1;
            new_pos.y = min_bound.y;
        }

        pos->pos = new_pos;
    }
}
