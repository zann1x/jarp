#ifndef VEC3_H
#define VEC3_H

#include "jarp/api_types.h"
#include "jarp/math/math.h"

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
typedef union Vec3f Vec3f;

/*
====================
math_vec3fv
====================
*/
inline static Vec3f math_vec3fv(float value) {
    Vec3f result = {
        .x = value,
        .y = value,
        .z = value
    };
    return result;
}

/*
====================
math_vec3fx
====================
*/
inline static Vec3f math_vec3fx(float val1, float val2, float val3) {
    Vec3f result = {
        .x = val1,
        .y = val2,
        .z = val3
    };
    return result;
}

/*
====================
math_vec3f_add
====================
*/
inline static Vec3f math_vec3f_add(const Vec3f vec1, const Vec3f vec2) {
    Vec3f result;
    result.x = vec1.x + vec2.x;
    result.y = vec1.y + vec2.y;
    result.z = vec1.z + vec2.z;
    return result;
}

/*
====================
math_vec3f_substract
====================
*/
// TODO: why the fuck can't i pass it by const reference here?
inline static Vec3f math_vec3f_substract(const Vec3f vec1, const Vec3f vec2) {
    Vec3f result = {
        .x = vec1.x - vec2.x,
        .y = vec1.y - vec2.y,
        .z = vec1.z - vec2.z
    };
    return result;
}

/*
====================
math_vec3f_multiply
====================
*/
// TODO: why the fuck can't i pass it by const reference here?
inline static Vec3f math_vec3f_multiply(const Vec3f vec, const float val) {
    Vec3f result = {
        .x = vec.x * val,
        .y = vec.y * val,
        .z = vec.z * val
    };
    return result;
}

/*
====================
math_vec3f_normalize
====================
*/
inline static Vec3f math_vec3f_normalize(const Vec3f vec) {
    const float length = sqrtf((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
    Vec3f result = {
        .x = vec.x / length,
        .y = vec.y / length,
        .z = vec.z / length
    };
    return result;
}

/*
====================
math_vec3f_cross
====================
*/
inline static Vec3f math_vec3f_cross(const Vec3f vec1, const Vec3f vec2) {
    Vec3f result = {
        .x = vec1.y * vec2.z - vec1.z * vec2.y,
        .y = vec1.z * vec2.x - vec1.x * vec2.z,
        .z = vec1.x * vec2.y - vec1.y * vec2.x
    };
    return result;
}

/*
====================
math_vec3f_dot
====================
*/
inline static float math_vec3f_dot(const Vec3f vec1, const Vec3f vec2) {
    return (vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z);
}

/*
====================
math_vec3f_negate
====================
*/
inline static Vec3f math_vec3f_negate(const Vec3f vec) {
    Vec3f result = {
        .x = -vec.x,
        .y = -vec.y,
        .z = -vec.z
    };
    return result;
}

#endif
