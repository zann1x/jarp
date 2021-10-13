#ifndef RENDERER_H
#define RENDERER_H

#include "Shader.h"

class Renderer {
public:
    Renderer();

    void load_sample_render_data();
    void draw(float delta);

private:
    Shader shader;
};

#endif
