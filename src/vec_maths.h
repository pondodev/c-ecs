#ifndef VEC_MATHS_H
#define VEC_MATHS_H

#include "ecs.h"

Vec2 vec2_add(Vec2 a, Vec2 b);
Vec2 vec2_sub(Vec2 a, Vec2 b);
float vec2_dot(Vec2 a, Vec2 b);
Vec2 vec2_mul(Vec2 a, float scalar);
Vec2 vec2_invert(Vec2 a);
Vec2 vec2_square(Vec2 a);

#endif // #ifndef VEC_MATHS_H

