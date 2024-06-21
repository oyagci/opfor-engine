#vertex
#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 tex_coords;
layout (location = 3) in vec4 in_tangent;
layout (location = 4) in float in_material;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 viewProjectionMatrix;
uniform mat4 modelMatrix = mat4(1.0);
uniform mat4 meshMatrix = mat4(1.0);
uniform vec3 viewPos;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out mat3 TBN;
out float MaterialID;

void main()
{
  mat4 meshAndModelMat = modelMatrix * meshMatrix;
	gl_Position = viewProjectionMatrix * meshAndModelMat * vec4(in_position, 1.0);
	FragPos = vec3(meshAndModelMat * vec4(in_position, 1.0));
	TexCoords = tex_coords;
	Normal = mat3(transpose(inverse(meshAndModelMat))) * in_normal;
	MaterialID = in_material;

	vec3 T = normalize(vec3(meshAndModelMat * in_tangent));
	vec3 N = normalize(vec3(meshAndModelMat * vec4(in_normal, 0.0)));
	vec3 B = normalize(cross(N, T)) * in_tangent.w;
	TBN = mat3(T, B, N);
}

#fragment
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
	if (tex.a < 0.0001)
		discard ;

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
