#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

out vec4 frag_color;

uniform vec3 lightColor;
uniform vec3 lightDir;
uniform vec3 lightPos;
uniform vec3 viewPos;

struct DirectionalLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct Material {
	sampler2D diffuse;  // 0
	sampler2D specular; // 1
	sampler2D normal;   // 2
	float shininess;
};

#define MAX_NUM_POINT_LIGHTS	16

uniform PointLight pointLight[MAX_NUM_POINT_LIGHTS];
uniform int pointLightCount;
uniform Material material;

//vec4 CalcDirectionalLight(DirectionalLight light, vec3 normal)
//{
//	/// Ambient
//	float ambient_strength = 0.2;
//	vec3 ambient_light = ambient_strength * light_color;
//
//	/// Diffuse
//	float diffuse_strength = max(dot(normal, -light_dir), 0.0);
//	vec3 diffuse_light = diffuse_strength * light_color;
//
//	/// Specular
//	vec3 view_dir = normalize(viewPos - FragPos);
//	vec3 reflect_dir = normalize(reflect(-light_dir, normal));
//	float specular_strength = 0.5;
//	float specular = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
//	vec3 specular_light = specular_strength * specular * light_color;
//
//	vec4 light = vec4(ambient_light + diffuse_light + specular_light, 1.0);
//
//	return light;
//}

vec3 CalcPointLight(PointLight light, vec3 normal, Material material)
{
	float dist = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	vec3 light_dir = normalize(light.position - FragPos);

	/// Ambient
	float ambient_strength = 0.1;
	vec3 ambient_light = ambient_strength * light.ambient;

	/// Diffuse
	float diffuse_strength = max(dot(normal, light_dir), 0.0);
	vec3 diffuse_light = diffuse_strength * light.diffuse;

	/// Specular
	vec3 view_dir = normalize(viewPos - FragPos);
	vec3 reflect_dir = normalize(reflect(-light_dir, normal));
	float specular_strength = 0.5;
	float specular = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
	vec3 specular_light = vec3(texture(material.specular, TexCoords)) * specular * light.specular;

	ambient_light *= attenuation;
	diffuse_light *= attenuation;
	specular_light *= attenuation;

	vec3 result = vec3(ambient_light + diffuse_light + specular_light);

	return result;
}

void main()
{
	vec3 normal = normalize(Normal);
//	vec4 light = CalcDirectionalLight(lightColor, lightDir, normal);

	normal = texture(material.normal, TexCoords).rgb;
	normal = normal * 2.0 - 1.0;
	normal = normalize(TBN * normal);

	vec3 lightResult = vec3(0.0, 0.0, 0.0);
	for (int i = 0; i < pointLightCount; i++) {
		lightResult += CalcPointLight(pointLight[i], normal, material);
	}

	vec4 tex = texture(material.diffuse, TexCoords);
	if (tex.a < 0.0001)
		discard ;

	frag_color = texture(material.diffuse, TexCoords) * vec4(lightResult, 1.0);
}
