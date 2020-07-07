#ifndef CAMERA_H
#define CAMERA_H

#include "jarp/api_types.h"

void camera_init(void);
void camera_update(uint16_t* input_key_down);
struct Mat4f camera_get_projection_view_matrix(void);

#endif
