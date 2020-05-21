#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 frag_color;

uniform vec3 lightColor;
uniform vec3 lightDir;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D tex;

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

uniform PointLight pointLight;

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

vec4 CalcPointLight(PointLight light, vec3 normal)
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
	vec3 specular_light = specular_strength * specular * light.specular;

	ambient_light *= attenuation;
	diffuse_light *= attenuation;
	specular_light *= attenuation;

	vec4 result = vec4(ambient_light + diffuse_light + specular_light, 1.0);

	return result;
}

void main()
{
	vec3 normal = normalize(Normal);
//	vec4 light = CalcDirectionalLight(lightColor, lightDir, normal);
	vec4 light = CalcPointLight(pointLight, normal);

	frag_color = texture(tex, TexCoords) * light;
}
