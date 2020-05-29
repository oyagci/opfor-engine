#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

struct Material {
	sampler2D diffuse;  // 0
	sampler2D specular; // 1
	sampler2D normal;   // 2
	float shininess;
};

uniform Material material;
uniform samplerCube depthMap;

void main()
{
	vec3 normal = normalize(Normal);

	normal = texture(material.normal, TexCoords).rgb;
	normal = normal * 2.0 - 1.0;
	normal = normalize(TBN * normal);

	vec4 tex = texture(material.diffuse, TexCoords);
	if (tex.a < 0.0001)
		discard ;

	gPosition = FragPos;
	gNormal = normal;
	gAlbedoSpec.rgb = texture(material.diffuse, TexCoords).rgb;
	gAlbedoSpec.a = texture(material.specular, TexCoords).r;
}
