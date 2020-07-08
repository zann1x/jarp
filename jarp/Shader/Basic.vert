#version 450

layout(binding = 0) uniform UniformBufferObject
{
    mat4 proj_view;
    mat4 model;
    vec3 light;
} ubo;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in vec2 texture_coordinate;

layout(location = 0) out vec3 v_frag_color;
layout(location = 1) out vec2 v_frag_texture_coordinate;

out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
    gl_Position = ubo.proj_view * ubo.model * vec4(position, 1.0);
    v_frag_color = color;
    v_frag_texture_coordinate = texture_coordinate;
}
