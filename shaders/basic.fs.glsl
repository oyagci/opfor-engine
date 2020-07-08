#version 330 core

#define NUM_MATERIALS	1

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec4 gMetallicRoughness;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;
in float MaterialID;

struct Material {
	vec4 baseColor;
	sampler2D albedo;
	sampler2D metallicRoughness;
	sampler2D normal;
	float metallicFactor;
	float roughnessFactor;

	bool hasAlbedo;
	bool hasMetallicRoughness;
};

uniform Material materials[NUM_MATERIALS];
uniform Material material;
uniform samplerCube depthMap;

void main()
{
	vec3 normal = normalize(Normal);

	normal = texture(material.normal, TexCoords).rgb;
	normal = normal * 2.0 - 1.0;
	normal = normalize(TBN * normal);

	vec4 tex = texture(material.albedo, TexCoords);
//	vec4 tex = texture(materials[int(MaterialID)].diffuse, TexCoords);

	gPosition = FragPos;
	gNormal = normal;
	if (material.hasAlbedo) {
		gAlbedoSpec.rgb = texture(material.albedo, TexCoords).rgb * material.baseColor.rgb;
	}
	else {
		gAlbedoSpec.rgb = material.baseColor.rgb;
	}
	gAlbedoSpec.a = 0.0;

	gMetallicRoughness = vec4(0.0);
	if (material.hasMetallicRoughness) {
		gMetallicRoughness.b = texture(material.metallicRoughness, TexCoords).b * material.metallicFactor;
		gMetallicRoughness.g = texture(material.metallicRoughness, TexCoords).g * material.roughnessFactor;
	}
	else {
		gMetallicRoughness.b = material.metallicFactor;
		gMetallicRoughness.g = material.roughnessFactor;
	}
}
