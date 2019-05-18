#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D TextureSampler;

layout(location = 0) in vec3 v_FragColor;
layout(location = 1) in vec2 v_FragTextureCoordinate;
layout(location = 2) in vec3 v_FragNormal;
layout(location = 3) in vec3 v_FragView;
layout(location = 4) in vec3 v_FragLight;

layout(location = 0) out vec4 OutColor;

void main()
{
	vec3 Normal = normalize(v_FragNormal);
	vec3 Light = normalize(v_FragLight);
	vec3 View = normalize(v_FragView);
	vec3 Reflect = reflect(-Light, Normal);
	float Shininess = 16.0;

	vec3 Ambient = v_FragColor * 0.1;
	vec3 Diffuse = max(dot(Normal, Light), 0.0) * v_FragColor;
	vec3 Specular = pow(max(dot(Reflect, View), 0.0f), Shininess) * vec3(1.35);

	OutColor = texture(TextureSampler, v_FragTextureCoordinate) * vec4(Ambient + Diffuse + Specular, 1.0);

	//float val = dot(Normal, View);
	//OutColor = vec4(val, val, val, 1.0);

	//OutColor = vec4(Normal, 1.0);
}
