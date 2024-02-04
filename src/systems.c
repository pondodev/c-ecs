#include "systems.h"

#include "ecs.h"
#include "raylib.h"

void system_draw(void) {
    DisplayComponent* displays = NULL;
    size_t display_count = 0;
    ecs_get_display_component_array(&displays, &display_count);

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
