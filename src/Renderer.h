#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include "Shader.h"

class Renderer {
public:
    Renderer();
    void load_sample_render_data();
    void draw(double delta);

private:
    std::unique_ptr<Shader> shader;
};

#endif
