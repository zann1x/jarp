#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject
{
	mat4 Model;
	mat4 View;
	mat4 Projection;
	vec3 LightPosition;
} ubo;

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Color;
layout(location = 2) in vec2 TextureCoordinate;
layout(location = 3) in vec3 Normal;

layout(location = 0) out vec3 v_FragColor;
layout(location = 1) out vec2 v_FragTextureCoordinate;
layout(location = 2) out vec3 v_FragNormal;
layout(location = 3) out vec3 v_FragView;
layout(location = 4) out vec3 v_FragLight;

out gl_PerVertex {
	vec4 gl_Position;
};

void main()
{
	vec4 WorldPosition = ubo.Model * vec4(Position, 1.0);
	gl_Position = ubo.Projection * ubo.View * WorldPosition;

	v_FragColor = Color;
	v_FragTextureCoordinate = TextureCoordinate;
	v_FragNormal = mat3(ubo.View) * mat3(ubo.Model) * Normal;
	v_FragView = -(ubo.View * WorldPosition).xyz;
	v_FragLight = mat3(ubo.View) * (ubo.LightPosition - WorldPosition.xyz);
}
