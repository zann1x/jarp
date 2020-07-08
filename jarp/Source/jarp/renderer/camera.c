#define CAMERA_SPEED 0.0005f

/*
====================
camera_init
====================
*/
void camera_init(struct Camera* camera) {
    camera->projection_matrix = math_mat4f_orthographic(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 100.0f);
    camera->view_matrix = math_mat4f_identity();
    camera->projection_view_matrix = math_mat4f_multiply(camera->projection_matrix, camera->view_matrix);
    camera->position = math_vec3fx(0.0f, 0.0f, -3.0f);
}

/*
====================
camera_update
====================
*/
void camera_update(struct Camera* camera, uint16_t* input_key_down) {
    static const Vec3f camera_up = { 0.0f, 1.0f, 0.0f };
    static const Vec3f camera_front = { 0.0f, 0.0f, 1.0f };

    const Vec3f right = math_vec3f_normalize(math_vec3f_cross(camera_up, camera_front));
    
    if (input_key_down[JARP_KEY_W]) {
        camera->position = math_vec3f_add(camera->position, math_vec3f_multiply(camera_up, CAMERA_SPEED));
    }
    else if (input_key_down[JARP_KEY_S]) {
        camera->position = math_vec3f_substract(camera->position, math_vec3f_multiply(camera_up, CAMERA_SPEED));
    }
    if (input_key_down[JARP_KEY_D]) {
        camera->position = math_vec3f_add(camera->position, math_vec3f_multiply(right, CAMERA_SPEED));
    }
    else if (input_key_down[JARP_KEY_A]) {
        camera->position = math_vec3f_substract(camera->position, math_vec3f_multiply(right, CAMERA_SPEED));
    }

    camera->view_matrix = math_mat4f_inverse(math_mat4f_translation(camera->position));
    camera->projection_view_matrix = math_mat4f_multiply(camera->projection_matrix, camera->view_matrix);
}
