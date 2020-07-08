#define CAMERA_SPEED 0.0005f

static Mat4f camera_projection = { 0.0f };
static Vec3f camera_position = { 0.0f };

static Vec3f camera_up = { 0.0f };
static Vec3f camera_front = { 0.0f };

/*
====================
camera_init
====================
*/
void camera_init(void) {
    camera_projection = math_mat4f_orthographic(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 100.0f);
    camera_position = math_vec3fx(0.0f, 0.0f, -3.0f);
    camera_up = math_vec3fx(0.0f, 1.0f, 0.0f);
    camera_front = math_vec3fx(0.0f, 0.0f, 1.0f);
}

/*
====================
camera_update
====================
*/
void camera_update(uint16_t* input_key_down) {
    const Vec3f right = math_vec3f_normalize(math_vec3f_cross(camera_up, camera_front));
    
    if (input_key_down[JARP_KEY_W]) {
        camera_position = math_vec3f_add(camera_position, math_vec3f_multiply(camera_up, CAMERA_SPEED));
    }
    else if (input_key_down[JARP_KEY_S]) {
        camera_position = math_vec3f_substract(camera_position, math_vec3f_multiply(camera_up, CAMERA_SPEED));
    }
    if (input_key_down[JARP_KEY_D]) {
        camera_position = math_vec3f_add(camera_position, math_vec3f_multiply(right, CAMERA_SPEED));
    }
    else if (input_key_down[JARP_KEY_A]) {
        camera_position = math_vec3f_substract(camera_position, math_vec3f_multiply(right, CAMERA_SPEED));
    }
}

/*
====================
camera_get_projection_view_matrix
====================
*/
Mat4f camera_get_projection_view_matrix(void) {
    // Invert the matrix as the objects in the world need to be moved in the opposite direction of the camera
    const Mat4f view = math_mat4f_inverse(math_mat4f_translation(camera_position));
    return math_mat4f_multiply(camera_projection, view);
}
