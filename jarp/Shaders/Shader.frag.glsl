#version 450

layout(binding = 1) uniform sampler2D TextureSampler;

layout(location = 0) in vec3 v_FragColor;
layout(location = 1) in vec2 v_FragTextureCoordinate;

layout(location = 0) out vec4 OutColor;

void main()
{
	OutColor = vec4(v_FragColor, 1.0);
	//OutColor = vec4(v_FragTextureCoordinate, 0.0, 1.0);
	//OutColor = texture(TextureSampler, v_FragTextureCoordinate);
}
