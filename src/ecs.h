#ifndef ECS_H
#define ECS_H

#include "raylib.h"
#include <stdlib.h>

typedef size_t EntityID;
#define INVALID_ENTITY_ID 0
#define COMPONENT_BASE EntityID owner;

typedef struct {
    float x;
    float y;
} Vec2;

typedef struct {
    COMPONENT_BASE
    Vec2    pos;
} PositionComponent;

typedef struct {
    COMPONENT_BASE
    float   radius;
    Color   color;
} DisplayComponent;

typedef struct {
    COMPONENT_BASE
    float   mass;
    Vec2    velocity;
} RigidBodyComponent;

typedef struct {
    COMPONENT_BASE
    float   radius;
} CircleColliderComponent;

void ecs_init(const size_t max_components);
void ecs_free(void);

void ecs_lock_mutex(void);
void ecs_unlock_mutex(void);

EntityID ecs_new_entity(void);

PositionComponent* ecs_new_position_component(const EntityID entity_id);
DisplayComponent* ecs_new_display_component(const EntityID entity_id);
RigidBodyComponent* ecs_new_rigid_body_component(const EntityID entity_id);
CircleColliderComponent* ecs_new_circle_collider_component(const EntityID entity_id);

PositionComponent* ecs_get_position_component(const EntityID entity_id);
DisplayComponent* ecs_get_display_component(const EntityID entity_id);
RigidBodyComponent* ecs_get_rigid_body_component(const EntityID entity_id);
CircleColliderComponent* ecs_get_circle_collider_component(const EntityID entity_id);

void ecs_get_position_component_array(PositionComponent** o_array, size_t* o_size);
void ecs_get_display_component_array(DisplayComponent** o_array, size_t* o_size);
void ecs_get_rigid_body_component_array(RigidBodyComponent** o_array, size_t* o_size);
void ecs_get_circle_collider_component_array(CircleColliderComponent** o_array, size_t* o_size);

#endif // #ifndef ECS_H

