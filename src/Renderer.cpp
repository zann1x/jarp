#include "Renderer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

GLuint vao;

Renderer::Renderer()
    : shader{ "../shaders/basic.vert", "../shaders/basic.frag" }
{
    glClearColor(0.f, 0.f, 0.f, 1.f);
}

void Renderer::load_sample_render_data() {
    GLfloat vertices[] = {
        // vec3 position    // vec4 color
        -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // left
         0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // right
         0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f  // top
    };

    GLuint vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), reinterpret_cast<const void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    GLuint indices[] = {
        0, 1, 2
    };
    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), static_cast<const void*>(indices), GL_STATIC_DRAW);
}

void Renderer::draw(double delta) {
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 projection = glm::ortho(-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    //glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) 800 / (float) 600, 0.1f, 100.0f);
    //glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) 800 / (float) 600, -1.0f, 1.0f);
    //glm::mat4 view = glm::lookAt(
    //        glm::vec3(0, 0, -1),
    //        glm::vec3(0, 0, 0),
    //        glm::vec3(0, -1, 0)
    //);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = projection * view * model;

    this->shader.bind();
    this->shader.set_mat4("mvp", mvp);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
}
