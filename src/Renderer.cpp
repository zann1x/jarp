#include "Renderer.h"
#include <glad/glad.h>
#include <string>
#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>

#include "FileSystem.h"

unsigned int shader_program;
unsigned int vao;

#define BUFFER_SIZE 1024
#define MAX_INFO_LOG_LENGTH 512

Renderer::Renderer() {
    glClearColor(0.f, 0.f, 0.f, 1.f);
}

void Renderer::load_sample_render_data() {
    std::string vertex_string = FileSystem::load_as_string("../shaders/basic.vert");
    std::string fragment_string = FileSystem::load_as_string("../shaders/basic.frag");
    const GLchar *vertex_shader_source = vertex_string.c_str();
    const GLchar *fragment_shader_source = fragment_string.c_str();

    int success;
    char info_log[MAX_INFO_LOG_LENGTH];

    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, MAX_INFO_LOG_LENGTH, nullptr, info_log);
        SPDLOG_ERROR("Vertex shader compilation failed: {}", info_log);
    }
    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, MAX_INFO_LOG_LENGTH, nullptr, info_log);
        SPDLOG_ERROR("Fragment shader compilation failed: {}", info_log);
    }

    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, MAX_INFO_LOG_LENGTH, nullptr, info_log);
        SPDLOG_ERROR("Shader program linking failed: {}", info_log);
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

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

    glUseProgram(shader_program);
    int matrix_id = glGetUniformLocation(shader_program, "mvp");
    glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
