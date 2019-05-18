#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject
{
	mat4 MVP;
} ubo;

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Color;
layout(location = 2) in vec2 TextureCoordinate;

layout(location = 0) out vec3 v_FragColor;
layout(location = 1) out vec2 v_FragTextureCoordinate;

out gl_PerVertex {
	vec4 gl_Position;
};

void main()
{
	gl_Position = ubo.MVP * vec4(Position, 1.0);
	v_FragColor = Color;
	v_FragTextureCoordinate = TextureCoordinate;
}
