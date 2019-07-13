#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject
{
	mat4 Model;
	mat4 View;
	mat4 Projection;
	vec3 LightPosition;
} ubo;

layout(location = 0) in vec3 InPosition;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec3 InColor;
layout(location = 3) in vec2 InTextureCoordinate;

layout(location = 0) out vec3 passSurfaceNormal;
layout(location = 1) out vec3 passColor;
layout(location = 2) out vec2 passTextureCoordinate;
layout(location = 3) out vec3 passView;
layout(location = 4) out vec3 passLight;

out gl_PerVertex {
	vec4 gl_Position;
};

void main()
{
	vec4 WorldPosition = ubo.Model * vec4(InPosition, 1.0);
	gl_Position = ubo.Projection * ubo.View * WorldPosition;

	passColor = InColor;
	passTextureCoordinate = InTextureCoordinate;
	passSurfaceNormal = mat3(ubo.View) * mat3(ubo.Model) * InNormal;
	passView = -(ubo.View * WorldPosition).xyz;
	passLight = mat3(ubo.View) * (ubo.LightPosition - WorldPosition.xyz);
}
