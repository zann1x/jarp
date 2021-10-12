#ifndef RENDERER_H
#define RENDERER_H

#include "Shader.h"

class Renderer {
public:
    Renderer();

    void load_sample_render_data();
    void draw(double delta);

private:
    Shader shader;
};

#endif
