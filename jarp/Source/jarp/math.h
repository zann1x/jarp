#pragma once

#include "api_types.h"

inline static uint32_t math_max(uint32_t num1, uint32_t num2) {
    return (num1 > num2 ? num1 : num2);
}

inline static uint32_t math_min(uint32_t num1, uint32_t num2) {
    return (num1 < num2 ? num1 : num2);
}

union Vec2f {
    struct {
        float x, y;
    };
    struct {
        float u, v;
    };
};

union Vec3f {
    struct {
        float x, y, z;
    };
    struct {
        float u, v, w;
    };
    struct {
        float r, g, b;
    };
};

typedef union Vec2f Vec2f;
typedef union Vec3f Vec3f;

// TODO
struct Mat4f {
    float tmp;
};

typedef struct Mat4f Mat4f;

inline static Vec2f math_vec2f_add(Vec2f vec1, Vec2f vec2) {
    Vec2f result;
    result.x = vec1.x + vec2.x;
    result.y = vec1.y + vec2.y;

    return result;
}

inline static Vec3f math_vec3f_add(Vec3f vec1, Vec3f vec2) {
    Vec3f result;
    result.x = vec1.x + vec2.x;
    result.y = vec1.y + vec2.y;
    result.z = vec1.z + vec2.z;

    return result;
}
