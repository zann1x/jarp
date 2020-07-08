#ifndef MATH_H
#define MATH_H

/*
====================
math_max
====================
*/
inline uint32_t math_max(uint32_t num1, uint32_t num2) {
    return (num1 > num2 ? num1 : num2);
}

/*
====================
math_min
====================
*/
inline uint32_t math_min(uint32_t num1, uint32_t num2) {
    return (num1 < num2 ? num1 : num2);
}

/*
============================================================
============================================================
============================================================
*/

union Vec2f {
    struct {
        float x, y;
    };
    struct {
        float u, v;
    };
};

typedef union Vec2f Vec2f;

/*
====================
math_vec2f_add
====================
*/
inline Vec2f math_vec2f_add(Vec2f vec1, Vec2f vec2) {
    Vec2f result;
    result.x = vec1.x + vec2.x;
    result.y = vec1.y + vec2.y;
    return result;
}

/*
============================================================
============================================================
============================================================
*/

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
inline Vec3f math_vec3fv(float value) {
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
inline Vec3f math_vec3fx(float val1, float val2, float val3) {
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
inline Vec3f math_vec3f_add(const Vec3f vec1, const Vec3f vec2) {
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
inline Vec3f math_vec3f_substract(const Vec3f vec1, const Vec3f vec2) {
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
inline Vec3f math_vec3f_multiply(const Vec3f vec, const float val) {
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
inline Vec3f math_vec3f_normalize(const Vec3f vec) {
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
inline Vec3f math_vec3f_cross(const Vec3f vec1, const Vec3f vec2) {
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
inline float math_vec3f_dot(const Vec3f vec1, const Vec3f vec2) {
    return (vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z);
}

/*
====================
math_vec3f_negate
====================
*/
inline Vec3f math_vec3f_negate(const Vec3f vec) {
    Vec3f result = {
        .x = -vec.x,
        .y = -vec.y,
        .z = -vec.z
    };
    return result;
}

/*
============================================================
============================================================
============================================================
*/

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
inline Mat4f math_mat4f_multiply(const Mat4f mat1, const Mat4f mat2) {
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
inline Mat4f math_mat4f_identity(void) {
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
inline Mat4f math_mat4f_translation(const Vec3f vec) {
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
inline Mat4f math_mat4f_rotation(float angle) {
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
inline Mat4f math_mat4f_orthographic(float left, float right, float bottom, float top, float here, float there) {
    Mat4f result = math_mat4f_identity();

    result.elements[0 + 0 * 4] = 2.0f / (right - left);

    result.elements[1 + 1 * 4] = 2.0f / (top - bottom);

#ifdef LEFT_HAND_COORDINATES
# ifdef ZERO_TO_ONE_COORDINATES
    result.elements[2 + 2 * 4] = 1.0f / (there - here);
# else
    result.elements[2 + 2 * 4] = 2.0f / (there - here);
# endif
#else
# ifdef ZERO_TO_ONE_COORDINATES
    result.elements[2 + 2 * 4] = -1.0f / (there - here);
# else
    result.elements[2 + 2 * 4] = -2.0f / (there - here);
# endif
#endif

    result.elements[0 + 3 * 4] = -(right + left) / (right - left);
    result.elements[1 + 3 * 4] = -(top + bottom) / (top - bottom);
#ifdef ZERO_TO_ONE_COORDINATES
    result.elements[2 + 3 * 4] = -(here) / (there - here);
#else
    result.elements[2 + 3 * 4] = -(there + here) / (there - here);
#endif

    return result;
}

/*
====================
math_mat4f_look_at

See https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/lookat-function
====================
*/
inline Mat4f math_mat4f_look_at(const Vec3f eye, const Vec3f target, const Vec3f up) {
    Mat4f result = math_mat4f_identity();

    Vec3f f = math_vec3f_normalize(math_vec3f_substract(target, eye)); // forward
#ifdef LEFT_HAND_COORDINATES
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

#ifdef LEFT_HAND_COORDINATES
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
#ifdef LEFT_HAND_COORDINATES
    result.elements[3 + 2 * 4] = -math_vec3f_dot(f, eye);
#else
    result.elements[3 + 2 * 4] = math_vec3f_dot(f, eye);
#endif

    return result;
}

/*
====================
math_mat4f_inverse

See https://stackoverflow.com/a/1148405/8313359
====================
*/
inline Mat4f math_mat4f_inverse(const Mat4f mat) {
    float inverse[16];

    inverse[0] = (mat.elements[5] * mat.elements[10] * mat.elements[15]) -
        (mat.elements[5] * mat.elements[11] * mat.elements[14]) -
        (mat.elements[9] * mat.elements[6] * mat.elements[15]) +
        (mat.elements[9] * mat.elements[7] * mat.elements[14]) +
        (mat.elements[13] * mat.elements[6] * mat.elements[11]) -
        (mat.elements[13] * mat.elements[7] * mat.elements[10]);

    inverse[4] = (-mat.elements[4] * mat.elements[10] * mat.elements[15]) +
        (mat.elements[4] * mat.elements[11] * mat.elements[14]) +
        (mat.elements[8] * mat.elements[6] * mat.elements[15]) -
        (mat.elements[8] * mat.elements[7] * mat.elements[14]) -
        (mat.elements[12] * mat.elements[6] * mat.elements[11]) +
        (mat.elements[12] * mat.elements[7] * mat.elements[10]);

    inverse[8] = (mat.elements[4] * mat.elements[9] * mat.elements[15]) -
        (mat.elements[4] * mat.elements[11] * mat.elements[13]) -
        (mat.elements[8] * mat.elements[5] * mat.elements[15]) +
        (mat.elements[8] * mat.elements[7] * mat.elements[13]) +
        (mat.elements[12] * mat.elements[5] * mat.elements[11]) -
        (mat.elements[12] * mat.elements[7] * mat.elements[9]);

    inverse[12] = (-mat.elements[4] * mat.elements[9] * mat.elements[14]) +
        (mat.elements[4] * mat.elements[10] * mat.elements[13]) +
        (mat.elements[8] * mat.elements[5] * mat.elements[14]) -
        (mat.elements[8] * mat.elements[6] * mat.elements[13]) -
        (mat.elements[12] * mat.elements[5] * mat.elements[10]) +
        (mat.elements[12] * mat.elements[6] * mat.elements[9]);

    inverse[1] = (-mat.elements[1] * mat.elements[10] * mat.elements[15]) +
        (mat.elements[1] * mat.elements[11] * mat.elements[14]) +
        (mat.elements[9] * mat.elements[2] * mat.elements[15]) -
        (mat.elements[9] * mat.elements[3] * mat.elements[14]) -
        (mat.elements[13] * mat.elements[2] * mat.elements[11]) +
        (mat.elements[13] * mat.elements[3] * mat.elements[10]);

    inverse[5] = (mat.elements[0] * mat.elements[10] * mat.elements[15]) -
        (mat.elements[0] * mat.elements[11] * mat.elements[14]) -
        (mat.elements[8] * mat.elements[2] * mat.elements[15]) +
        (mat.elements[8] * mat.elements[3] * mat.elements[14]) +
        (mat.elements[12] * mat.elements[2] * mat.elements[11]) -
        (mat.elements[12] * mat.elements[3] * mat.elements[10]);

    inverse[9] = (-mat.elements[0] * mat.elements[9] * mat.elements[15]) +
        (mat.elements[0] * mat.elements[11] * mat.elements[13]) +
        (mat.elements[8] * mat.elements[1] * mat.elements[15]) -
        (mat.elements[8] * mat.elements[3] * mat.elements[13]) -
        (mat.elements[12] * mat.elements[1] * mat.elements[11]) +
        (mat.elements[12] * mat.elements[3] * mat.elements[9]);

    inverse[13] = (mat.elements[0] * mat.elements[9] * mat.elements[14]) -
        (mat.elements[0] * mat.elements[10] * mat.elements[13]) -
        (mat.elements[8] * mat.elements[1] * mat.elements[14]) +
        (mat.elements[8] * mat.elements[2] * mat.elements[13]) +
        (mat.elements[12] * mat.elements[1] * mat.elements[10]) -
        (mat.elements[12] * mat.elements[2] * mat.elements[9]);

    inverse[2] = (mat.elements[1] * mat.elements[6] * mat.elements[15]) -
        (mat.elements[1] * mat.elements[7] * mat.elements[14]) -
        (mat.elements[5] * mat.elements[2] * mat.elements[15]) +
        (mat.elements[5] * mat.elements[3] * mat.elements[14]) +
        (mat.elements[13] * mat.elements[2] * mat.elements[7]) -
        (mat.elements[13] * mat.elements[3] * mat.elements[6]);

    inverse[6] = (-mat.elements[0] * mat.elements[6] * mat.elements[15]) +
        (mat.elements[0] * mat.elements[7] * mat.elements[14]) +
        (mat.elements[4] * mat.elements[2] * mat.elements[15]) -
        (mat.elements[4] * mat.elements[3] * mat.elements[14]) -
        (mat.elements[12] * mat.elements[2] * mat.elements[7]) +
        (mat.elements[12] * mat.elements[3] * mat.elements[6]);

    inverse[10] = (mat.elements[0] * mat.elements[5] * mat.elements[15]) -
        (mat.elements[0] * mat.elements[7] * mat.elements[13]) -
        (mat.elements[4] * mat.elements[1] * mat.elements[15]) +
        (mat.elements[4] * mat.elements[3] * mat.elements[13]) +
        (mat.elements[12] * mat.elements[1] * mat.elements[7]) -
        (mat.elements[12] * mat.elements[3] * mat.elements[5]);

    inverse[14] = (-mat.elements[0] * mat.elements[5] * mat.elements[14]) +
        (mat.elements[0] * mat.elements[6] * mat.elements[13]) +
        (mat.elements[4] * mat.elements[1] * mat.elements[14]) -
        (mat.elements[4] * mat.elements[2] * mat.elements[13]) -
        (mat.elements[12] * mat.elements[1] * mat.elements[6]) +
        (mat.elements[12] * mat.elements[2] * mat.elements[5]);

    inverse[3] = (-mat.elements[1] * mat.elements[6] * mat.elements[11]) +
        (mat.elements[1] * mat.elements[7] * mat.elements[10]) +
        (mat.elements[5] * mat.elements[2] * mat.elements[11]) -
        (mat.elements[5] * mat.elements[3] * mat.elements[10]) -
        (mat.elements[9] * mat.elements[2] * mat.elements[7]) +
        (mat.elements[9] * mat.elements[3] * mat.elements[6]);

    inverse[7] = (mat.elements[0] * mat.elements[6] * mat.elements[11]) -
        (mat.elements[0] * mat.elements[7] * mat.elements[10]) -
        (mat.elements[4] * mat.elements[2] * mat.elements[11]) +
        (mat.elements[4] * mat.elements[3] * mat.elements[10]) +
        (mat.elements[8] * mat.elements[2] * mat.elements[7]) -
        (mat.elements[8] * mat.elements[3] * mat.elements[6]);

    inverse[11] = (-mat.elements[0] * mat.elements[5] * mat.elements[11]) +
        (mat.elements[0] * mat.elements[7] * mat.elements[9]) +
        (mat.elements[4] * mat.elements[1] * mat.elements[11]) -
        (mat.elements[4] * mat.elements[3] * mat.elements[9]) -
        (mat.elements[8] * mat.elements[1] * mat.elements[7]) +
        (mat.elements[8] * mat.elements[3] * mat.elements[5]);

    inverse[15] = (mat.elements[0] * mat.elements[5] * mat.elements[10]) -
        (mat.elements[0] * mat.elements[6] * mat.elements[9]) -
        (mat.elements[4] * mat.elements[1] * mat.elements[10]) +
        (mat.elements[4] * mat.elements[2] * mat.elements[9]) +
        (mat.elements[8] * mat.elements[1] * mat.elements[6]) -
        (mat.elements[8] * mat.elements[2] * mat.elements[5]);

    float determinant = mat.elements[0] * inverse[0] +
        mat.elements[1] * inverse[4] +
        mat.elements[2] * inverse[8] +
        mat.elements[3] * inverse[12];

    Mat4f result = { 0.0f };
    if (determinant != 0) {
        determinant = 1.0f / determinant;

        for (uint32_t i = 0; i < 16; i++) {
            result.elements[i] = inverse[i] * determinant;
        }
    }
    return result;
}

#endif
