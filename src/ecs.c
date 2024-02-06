#include "ecs.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct {
    PositionComponent*          positions;
    size_t                      position_count;
    DisplayComponent*           displays;
    size_t                      display_count;
    RigidBodyComponent*         rigid_bodies;
    size_t                      rigid_body_count;
    CircleColliderComponent*    circle_colliders;
    size_t                      circle_collider_count;
} Components;

static EntityID         s_next_entity       = 1;
static Components       s_components;
static size_t           s_max_components    = 0;
static unsigned char*   s_components_buffer = NULL;
static pthread_mutex_t  s_lock;

static void* _get_component(void* component_array, size_t stride, EntityID entity_id);
static void* _get_free_component(void* component_array, size_t stride);

void ecs_init(const size_t max_components) {
    if (s_components_buffer != NULL)
        return;

    pthread_mutex_init(&s_lock, NULL);

    memset(&s_components, 0, sizeof(s_components));

    s_max_components = max_components;
    const size_t positions_array_size           = sizeof(PositionComponent)*s_max_components;
    const size_t displays_array_size            = sizeof(DisplayComponent)*s_max_components;
    const size_t rigid_bodies_array_size        = sizeof(RigidBodyComponent)*s_max_components;
    const size_t circle_colliders_array_size    = sizeof(CircleColliderComponent)*s_max_components;

    // allocate a shared buffer for all components to live in and zero it out
    const size_t total_buffer_size = positions_array_size + displays_array_size + rigid_bodies_array_size + circle_colliders_array_size;
    s_components_buffer = malloc(total_buffer_size);
    memset(s_components_buffer, 0, total_buffer_size);

    // allocate a portion of the shared buffer to each component array
    unsigned char* buffer_offset = s_components_buffer;
    s_components.positions = (void*)buffer_offset;
    buffer_offset += positions_array_size;

    s_components.displays = (void*)buffer_offset;
    buffer_offset += displays_array_size;

    s_components.rigid_bodies = (void*)buffer_offset;
    buffer_offset += rigid_bodies_array_size;

    s_components.circle_colliders = (void*)buffer_offset;
    buffer_offset += circle_colliders_array_size;

    for (size_t i = 0; i < s_max_components; ++i) {
        s_components.positions[i].owner         = INVALID_ENTITY_ID;
        s_components.displays[i].owner          = INVALID_ENTITY_ID;
        s_components.rigid_bodies[i].owner      = INVALID_ENTITY_ID;
        s_components.circle_colliders[i].owner  = INVALID_ENTITY_ID;
    }
}

void ecs_free(void) {
    if (s_components_buffer == NULL)
        return;

    free(s_components_buffer);
}

void ecs_lock_mutex(void) {
    pthread_mutex_lock(&s_lock);
}

void ecs_unlock_mutex(void) {
    pthread_mutex_unlock(&s_lock);
}

EntityID ecs_new_entity(void) {
    const EntityID new_entity = s_next_entity++;
    return new_entity;
}

PositionComponent* ecs_new_position_component(const EntityID entity_id) {
    if (s_components.position_count == s_max_components)
        return NULL;

    PositionComponent* c = _get_free_component(s_components.positions, sizeof(PositionComponent));
    c->owner = entity_id;
    s_components.position_count++;

    return c;
}

DisplayComponent* ecs_new_display_component(const EntityID entity_id) {
    if (s_components.display_count == s_max_components)
        return NULL;

    DisplayComponent* c = _get_free_component(s_components.displays, sizeof(DisplayComponent));
    c->owner = entity_id;
    s_components.display_count++;

    return c;
}

RigidBodyComponent* ecs_new_rigid_body_component(const EntityID entity_id) {
    if (s_components.rigid_body_count == s_max_components)
        return NULL;

    RigidBodyComponent* c = _get_free_component(s_components.rigid_bodies, sizeof(RigidBodyComponent));
    c->owner = entity_id;
    s_components.rigid_body_count++;

    return c;
}

CircleColliderComponent* ecs_new_circle_collider_component(const EntityID entity_id) {
    if (s_components.circle_collider_count == s_max_components)
        return NULL;

    CircleColliderComponent* c = _get_free_component(s_components.circle_colliders, sizeof(CircleColliderComponent));
    c->owner = entity_id;
    s_components.circle_collider_count++;

    return c;
}

PositionComponent* ecs_get_position_component(const EntityID entity_id) {
    return _get_component(s_components.positions, sizeof(PositionComponent), entity_id);
}

DisplayComponent* ecs_get_display_component(const EntityID entity_id) {
    return _get_component(s_components.displays, sizeof(DisplayComponent), entity_id);
}

RigidBodyComponent* ecs_get_rigid_body_component(const EntityID entity_id) {
    return _get_component(s_components.rigid_bodies, sizeof(RigidBodyComponent), entity_id);
}

CircleColliderComponent* ecs_get_circle_collider_component(const EntityID entity_id) {
    return _get_component(s_components.circle_colliders, sizeof(CircleColliderComponent), entity_id);
}

void ecs_get_position_component_array(PositionComponent** o_array, size_t* o_size) {
    *o_array = s_components.positions;
    *o_size = s_components.position_count;
}

void ecs_get_display_component_array(DisplayComponent** o_array, size_t* o_size) {
    *o_array = s_components.displays;
    *o_size = s_components.display_count;
}
void ecs_get_rigid_body_component_array(RigidBodyComponent** o_array, size_t* o_size) {
    *o_array = s_components.rigid_bodies;
    *o_size = s_components.rigid_body_count;
}
void ecs_get_circle_collider_component_array(CircleColliderComponent** o_array, size_t* o_size) {
    *o_array = s_components.circle_colliders;
    *o_size = s_components.circle_collider_count;
}

static void* _get_component(void* component_array, size_t stride, EntityID entity_id) {
    const unsigned char* start_addr = component_array;

    for (size_t i = 0; i < s_max_components; ++i) {
        const unsigned char* c = start_addr + (i * stride);
        const EntityID* id = (EntityID*)c;
        if (*id == entity_id)
            return (void*)c;
    }

    return NULL;
}

static void* _get_free_component(void* component_array, size_t stride) {
    return _get_component(component_array, stride, INVALID_ENTITY_ID);
}

