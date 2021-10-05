#include "Shader.h"
#include "FileSystem.h"
#include <spdlog/spdlog.h>
#include <glm/gtc/type_ptr.hpp>

#define MAX_INFO_LOG_LENGTH 512

Shader::Shader(const std::string& vertex_filepath, const std::string& fragment_filepath)
{
    // TODO: error handling
    std::string vertex_string = FileSystem::load_as_string("../shaders/basic.vert");
    std::string fragment_string = FileSystem::load_as_string("../shaders/basic.frag");
    const GLchar* vertex_shader_source = vertex_string.c_str();
    const GLchar* fragment_shader_source = fragment_string.c_str();

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

    program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader);
    glAttachShader(program_id, fragment_shader);
    glLinkProgram(program_id);
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program_id, MAX_INFO_LOG_LENGTH, nullptr, info_log);
        SPDLOG_ERROR("Shader program linking failed: {}", info_log);
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Shader::bind()
{
    glUseProgram(program_id);
}

void Shader::set_mat4(const std::string& name, const glm::mat4& value)
{
    GLint matrix_id = glGetUniformLocation(program_id, name.c_str());
    glUniformMatrix4fv(matrix_id, 1, GL_FALSE, glm::value_ptr(value));
}
