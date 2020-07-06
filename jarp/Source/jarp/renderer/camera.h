#ifndef CAMERA_H
#define CAMERA_H

void camera_init(void);
void camera_update(void);
struct Mat4f camera_get_projection_view_matrix(void);

#endif
