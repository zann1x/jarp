#pragma once

#include "jarp/api_types.h"
#include "jarp/math/math.h"
#include "jarp/math/vec3.h"

struct Mat4f {
    float elements[16];
};
typedef struct Mat4f Mat4f;

/*
====================
math_mat4f_multiply
====================
*/
inline static Mat4f math_mat4f_multiply(const Mat4f mat1, const Mat4f mat2) {
    Mat4f result = { 0.0f };

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            float sum = 0.0f;
            for (int e = 0; e < 4; e++) {
                sum += mat1.elements[x + e * 4] * mat2.elements[e + y * 4];
            }
            result.elements[x + y * 4] = sum;
        }
    }

    return result;
}

/*
====================
math_mat4f_identity
====================
*/
inline static Mat4f math_mat4f_identity(void) {
    Mat4f result;
    result.elements[0 + 0 * 4] = 1.0f;
    result.elements[1 + 1 * 4] = 1.0f;
    result.elements[2 + 2 * 4] = 1.0f;
    result.elements[3 + 3 * 4] = 1.0f;

    return result;
}

/*
====================
math_mat4f_translation
====================
*/
inline static Mat4f math_mat4f_translation(const Vec3f vec) {
    Mat4f result = math_mat4f_identity();

    result.elements[0 + 3 * 4] = vec.x;
    result.elements[1 + 3 * 4] = vec.y;
    result.elements[2 + 3 * 4] = vec.z;

    return result;
}

/*
====================
math_mat4f_rotation
====================
*/
inline static Mat4f math_mat4f_rotation(float angle) {
    Mat4f result = math_mat4f_identity();

    float rotation = (float)((angle * M_PI) / 180.0f);
    float cosine = (float)cos(rotation);
    float sine = (float)sin(rotation);

    result.elements[0 + 0 * 4] = cosine;
    result.elements[1 + 0 * 4] = sine;

    result.elements[0 + 1 * 4] = -sine;
    result.elements[1 + 1 * 4] = cosine;

    return result;
}

/*
====================
math_mat4f_orthographic
====================
*/
inline static Mat4f math_mat4f_orthographic(float left, float right, float bottom, float top, float near, float far) {
    Mat4f result = math_mat4f_identity();

    result.elements[0 + 0 * 4] = 2.0f / (right - left);

    result.elements[1 + 1 * 4] = 2.0f / (top - bottom);

    result.elements[2 + 2 * 4] = -2.0f / (far - near);

    result.elements[0 + 3 * 4] = -(right + left) / (right - left);
    result.elements[1 + 3 * 4] = -(top + bottom) / (top - bottom);
    result.elements[2 + 3 * 4] = -(far + near) / (far - near);

    // TODO?: Convert projection matrix to Vulkan coordinate system
    //        https://matthewwellings.com/blog/the-new-vulkan-coordinate-system/
    
    return result;
}

/*
====================
math_mat4f_look_at
====================
*/
inline static Mat4f math_mat4f_look_at(const Vec3f eye, const Vec3f target, const Vec3f up) {
    Mat4f result = math_mat4f_identity();

    Vec3f zaxis = math_vec3f_normalize(math_vec3f_substract(eye, target)); // forward
    Vec3f xaxis = math_vec3f_normalize(math_vec3f_cross(up, zaxis)); // right
    Vec3f yaxis = math_vec3f_normalize(math_vec3f_cross(zaxis, xaxis)); // up

    result.elements[0 + 0 * 4] = xaxis.x;
    result.elements[0 + 1 * 4] = xaxis.y;
    result.elements[0 + 2 * 4] = xaxis.z;
    result.elements[0 + 3 * 4] = math_vec3f_dot(math_vec3f_negate(xaxis), eye);

    result.elements[1 + 0 * 4] = yaxis.x;
    result.elements[1 + 1 * 4] = yaxis.y;
    result.elements[1 + 2 * 4] = yaxis.z;
    result.elements[1 + 3 * 4] = math_vec3f_dot(math_vec3f_negate(yaxis), eye);

    result.elements[2 + 0 * 4] = zaxis.x;
    result.elements[2 + 1 * 4] = zaxis.y;
    result.elements[2 + 2 * 4] = zaxis.z;
    result.elements[2 + 3 * 4] = math_vec3f_dot(math_vec3f_negate(zaxis), eye);

    return result;
}
