#version 450

layout(binding = 1) uniform sampler2D texture_sampler;

layout(location = 0) in vec3 v_frag_color;
layout(location = 1) in vec2 v_frag_texture_coordinate;

layout(location = 0) out vec4 out_color;

void main()
{
	out_color = vec4(v_frag_color, 1.0);
	//out_color = vec4(v_frag_texture_coordinate, 0.0, 1.0);
	//out_color = texture(texture_sampler, v_frag_texture_coordinate);
}
