#ifndef RENDERER_H
#define RENDERER_H

#include "Camera.h"
#include "Shader.h"

class Renderer {
public:
    Renderer();

    void load_sample_render_data();
    void draw(float delta, const Camera& camera);

private:
    Shader shader;
};

#endif
