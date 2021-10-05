#include "Renderer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

unsigned int vao;

Renderer::Renderer()
    : shader{ std::make_unique<Shader>("../shaders/basic.vert", "../shaders/basic.frag") }
{
    glClearColor(0.f, 0.f, 0.f, 1.f);
}

void Renderer::load_sample_render_data() {
    float vertices[] = {
            -0.5f, -0.5f, 0.0f, // left
            0.5f, -0.5f, 0.0f, // right
            0.0f, 0.5f, 0.0f  // top
    };

    unsigned int vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::draw(double delta) {
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) 800 / (float) 600, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(
            glm::vec3(0, 3, 3), // Camera is at (4,3,3), in world space
            glm::vec3(0, 0, 0), // Look at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = projection * view * model;

    shader->bind();
    shader->set_mat4("mvp", mvp);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
