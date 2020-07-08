#ifndef CAMERA_H
#define CAMERA_H

struct Camera {
    Mat4f projection_matrix;
    Mat4f view_matrix;
    Mat4f projection_view_matrix;
    Vec3f position;
};

void camera_init(struct Camera* camera);
void camera_update(struct Camera* camera, uint16_t* input_key_down);

#endif
