#include "Renderer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <stdexcept>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl.h>

GLuint vao;
GLuint vbo;

struct Vertex {
    glm::vec3 position;
    glm::vec4 color;
};

Renderer::Renderer()
    : shader{ "../shaders/basic.vert.glsl", "../shaders/basic.frag.glsl" }
{
    glClearColor(0.f, 0.f, 0.f, 1.f);
}

void Renderer::load_sample_render_data() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 1000 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, color)));
    glEnableVertexAttribArray(1);

    GLuint indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), static_cast<const void*>(indices), GL_STATIC_DRAW);
}

static std::array<Vertex, 4> create_quad(float x, float y) {
    float size = 1.0f;

    Vertex bottom_left = {
        .position = glm::vec3(x, y, 0.0f),
        .color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)
    };
    Vertex bottom_right = {
        .position = glm::vec3(x + size,  y, 0.0f),
        .color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)
    };
    Vertex top_right = {
        .position = glm::vec3(x + size, y - size, 0.0f),
        .color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
    };
    Vertex top_left = {
        .position = glm::vec3(x, y - size, 0.0f ),
        .color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
    };

    return { bottom_left, bottom_right, top_right, top_left };
}

static bool is_demo_window_open = true;

void Renderer::draw(double delta) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (is_demo_window_open) {
        ImGui::ShowDemoWindow(&is_demo_window_open);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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

    //GLfloat vertices[] = {
    //    // vec3 position    // vec4 color
    //    -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // bottom left
    //     0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // bottom right
    //     0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
    //    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f  // top left
    //};
    auto vertices = create_quad(-0.5f, 0.5f);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices.data());

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
