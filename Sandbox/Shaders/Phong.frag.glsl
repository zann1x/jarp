#version 450

layout(binding = 1) uniform sampler2D TextureSampler;

layout(location = 0) in vec3 passSurfaceNormal;
layout(location = 1) in vec3 passColor;
layout(location = 2) in vec2 passTextureCoordinate;
layout(location = 3) in vec3 passView;
layout(location = 4) in vec3 passLight;

layout(location = 0) out vec4 OutColor;

vec3 ads()
{
	vec3 unitNormal = normalize(passSurfaceNormal);
	vec3 unitLight = normalize(passLight);
	vec3 unitView = normalize(passView);
	//vec3 Reflect = reflect(-unitLight, unitNormal);
	vec3 HalfwayLightView = normalize(-unitLight + unitView);

	float AmbientReflectivity = 0.1;
	float Shininess = 16.0;
	float SpecularReflectivity = 1.35;
	vec3 LightIntensity = vec3(0.9);

	vec3 Ambient = AmbientReflectivity * passColor;

	float dotNormalLight = dot(unitNormal, unitLight);
	float Brightness = max(dotNormalLight, 0.2);
	vec3 Diffuse = Brightness * passColor;
	
	//float SpecularFactor = dot(Reflect, unitView);
	float SpecularFactor = dot(HalfwayLightView, unitNormal);
	SpecularFactor = max(SpecularFactor, 0.0);
	float DampedFactor = pow(SpecularFactor, Shininess);
	vec3 Specular = DampedFactor * SpecularReflectivity * passColor;

	return LightIntensity * (Ambient + Diffuse + Specular);
}

void main()
{
	//OutColor = vec4(ads(), 1.0) * texture(TextureSampler, passTextureCoordinate);
	OutColor = vec4(ads(), 1.0);

	//float val = dot(normalize(passSurfaceNormal), normalize(passView));
	//OutColor = vec4(val, val, val, 1.0);

	//OutColor = vec4(normalize(passSurfaceNormal), 1.0);
}
