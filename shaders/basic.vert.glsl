#version 460 core

layout (location = 0) in vec3 i_pos;
layout (location = 1) in vec4 i_color;
layout (location = 2) in vec2 i_texture;

uniform mat4 u_mvp;

out vec4 p_color;
out vec2 p_texture;

void main()
{
    p_color = i_color;
    p_texture = i_texture;
    gl_Position = u_mvp * vec4(i_pos.xyz, 1.0);
}
