#include "vec_maths.h"

Vec2 vec2_add(Vec2 a, Vec2 b) {
    return (Vec2) {
        .x = a.x + b.x,
        .y = a.y + b.y,
    };
}

Vec2 vec2_sub(Vec2 a, Vec2 b) {
    return (Vec2) {
        .x = a.x - b.x,
        .y = a.y - b.y,
    };
}

float vec2_dot(Vec2 a, Vec2 b) {
    return (a.x*b.x) + (a.y*b.y);
}

Vec2 vec2_mul(Vec2 a, float scalar) {
    return (Vec2) {
        .x = a.x * scalar,
        .y = a.y * scalar,
    };
}

Vec2 vec2_invert(Vec2 a) {
    return vec2_mul(a, -1);
}

Vec2 vec2_square(Vec2 a) {
    return (Vec2) {
        .x = a.x * a.x,
        .y = a.y * a.y,
    };
}

