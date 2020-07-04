#pragma once

#include "jarp/api_types.h"
#include "jarp/math/math.h"
#include "jarp/math/vec3.h"

/*
- Left Hand Coordinate System
- Depth zero to one
- Row major matrix: (a1, a2, a3, a4,
                     b1, b2, b3, b4,
                     c1, c2, c3, c4,
                     d1, d2, d3, d4)
  */
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
    Mat4f result = { 0.0f };
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

See https://solarianprogrammer.com/2013/05/22/opengl-101-matrices-projection-view-model/
====================
*/
inline static Mat4f math_mat4f_orthographic(float left, float right, float bottom, float top, float near, float far) {
    Mat4f result = math_mat4f_identity();

    result.elements[0 + 0 * 4] = 2.0f / (right - left);

    result.elements[1 + 1 * 4] = 2.0f / (top - bottom);

#if LEFT_HAND_COORDINATES
# if ZERO_TO_ONE_COORDINATES
    result.elements[2 + 2 * 4] = 1.0f / (far - near);
# else
    result.elements[2 + 2 * 4] = 2.0f / (far - near);
# endif
#else
# if ZERO_TO_ONE_COORDINATES
    result.elements[2 + 2 * 4] = -1.0f / (far - near);
# else
    result.elements[2 + 2 * 4] = -2.0f / (far - near);
# endif
#endif

    result.elements[0 + 3 * 4] = -(right + left) / (right - left);
    result.elements[1 + 3 * 4] = -(top + bottom) / (top - bottom);
#if ZERO_TO_ONE_COORDINATES
    result.elements[2 + 3 * 4] = -(near) / (far - near);
#else
    result.elements[2 + 3 * 4] = -(far + near) / (far - near);
#endif

    return result;
}

/*
====================
math_mat4f_look_at

See https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/lookat-function
====================
*/
inline static Mat4f math_mat4f_look_at(const Vec3f eye, const Vec3f target, const Vec3f up) {
    Mat4f result = math_mat4f_identity();

    Vec3f f = math_vec3f_normalize(math_vec3f_substract(target, eye)); // forward
#if LEFT_HAND_COORDINATES
    Vec3f s = math_vec3f_normalize(math_vec3f_cross(up, f)); // right
    Vec3f u = math_vec3f_cross(f, s); // up
#else
    Vec3f s = math_vec3f_normalize(math_vec3f_cross(f, up)); // right
    Vec3f u = math_vec3f_cross(s, f); // up
#endif

    result.elements[0 + 0 * 4] = s.x;
    result.elements[0 + 1 * 4] = s.y;
    result.elements[0 + 2 * 4] = s.z;

    result.elements[1 + 0 * 4] = u.x;
    result.elements[1 + 1 * 4] = u.y;
    result.elements[1 + 2 * 4] = u.z;

#if LEFT_HAND_COORDINATES
    result.elements[2 + 0 * 4] = f.x;
    result.elements[2 + 1 * 4] = f.y;
    result.elements[2 + 2 * 4] = f.z;
#else
    result.elements[2 + 0 * 4] = -f.x;
    result.elements[2 + 1 * 4] = -f.y;
    result.elements[2 + 2 * 4] = -f.z;
#endif

    result.elements[3 + 0 * 4] = -math_vec3f_dot(s, eye);
    result.elements[3 + 1 * 4] = -math_vec3f_dot(u, eye);
#if LEFT_HAND_COORDINATES
    result.elements[3 + 2 * 4] = -math_vec3f_dot(f, eye);
#else
    result.elements[3 + 2 * 4] = math_vec3f_dot(f, eye);
#endif

    return result;
}

/*
====================
math_mat4f_clip

See https://matthewwellings.com/blog/the-new-vulkan-coordinate-system/
====================
*/
//inline static Mat4f math_mat4f_clip(const Mat4f mat) {
//    Mat4f clip = math_mat4f_identity();
//    clip.elements[1 + 1 * 4] = -1.0f;
//    clip.elements[2 + 2 * 4] = 0.5f;
//    clip.elements[2 + 3 * 4] = 0.5f;
//    return math_mat4f_multiply(clip, mat);
//}
