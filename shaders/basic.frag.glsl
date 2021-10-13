#version 460 core

uniform sampler2D u_texture_position;

in vec4 p_color;
in vec2 p_texture;

out vec4 o_color;

void main()
{
    o_color = texture(u_texture_position, p_texture) * p_color;
}
