#pragma once

void camera_init(void);
void camera_update(void);
struct Mat4f camera_get_projection_view_matrix(void);
