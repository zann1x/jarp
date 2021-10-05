#ifndef RENDERER_H
#define RENDERER_H

class Renderer {
public:
    Renderer();
    void load_sample_render_data();
    void draw(double delta);
};

#endif
