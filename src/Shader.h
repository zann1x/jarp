#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
    Shader(const std::string& vertex_filepath, const std::string& fragment_filepath);

    void bind();

    void set_int(const std::string& name, GLint value);
    void set_vec4(const std::string& name, const glm::vec4& value);
    void set_mat4(const std::string& name, const glm::mat4& value);
private:
    GLuint program_id;
};

#endif
