#ifndef MESH_H
#define MESH_H

#include "api_types.h"

struct MeshVertex {
    float position[3];
    float uv[2];
    uint8_t color[4];
};

void hello(void) {
    int val = 3;
}

#endif
