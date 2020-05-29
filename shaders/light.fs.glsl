#version 450 core
#define MAX_NUM_POINT_LIGHTS	16

struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
	float radius;
};

out vec4 frag_color;

uniform vec3 viewPos;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gSSAO;
uniform samplerCube depthMap;

uniform PointLight pointLight[MAX_NUM_POINT_LIGHTS];
uniform int pointLightCount;

in vec2 TexCoords;

float CalcShadow(PointLight light, vec3 fragPos)
{
	float far_plane = 10000.0;
	vec3 fragToLight = fragPos - light.position;
	float currentDepth = length(fragToLight);
	float bias = 10.0;
	float shadow = 0.0;

	// PCF 
	float samples = 4.0;
	float off = 0.2;
	for (float x = -off; x < off; x += off / (samples * 0.5)) {
		for (float y = -off; y < off; y += off / (samples * 0.5)) {
			for (float z = -off; z < off; z += off / (samples * 0.5)) {
				float closestDepth = (texture(depthMap, fragToLight + vec3(x, y, z))).r * far_plane;
				shadow += (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
			}
		}
	}
	shadow /= (samples * samples * samples);

	return shadow;
}

vec3 CalcPointLight(PointLight light, vec3 fragPos, vec3 normal, float spec, float ao)
{
	float dist = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	vec3 light_dir = normalize(light.position - fragPos);

	/// Ambient
	float ambient_strength = 0.5;
	vec3 ambient_light = ambient_strength * light.ambient * ao;

	/// Diffuse
	float diffuse_strength = max(dot(normal, light_dir), 0.0);
	vec3 diffuse_light = diffuse_strength * light.diffuse;

	/// Specular
	vec3 view_dir = normalize(viewPos - fragPos);
	vec3 reflect_dir = normalize(reflect(-light_dir, normal));
	float specular = pow(max(dot(view_dir, reflect_dir), 0.0), 16);
	vec3 specular_light = spec * specular * light.specular;

	float shadow = CalcShadow(light, fragPos);

	ambient_light *= attenuation;
	diffuse_light *= attenuation;
	specular_light *= attenuation;

	vec3 result = vec3((ambient_light + (1.0 - shadow)) * (diffuse_light + specular_light));

	return result;
}

void main()
{
	vec3  fragPos = texture(gPosition, TexCoords).rgb;
	vec3  normal = texture(gNormal, TexCoords).rgb;
	vec3  albedo = texture(gAlbedoSpec, TexCoords).rgb;
	float specular = texture(gAlbedoSpec, TexCoords).a;
	float ao = texture(gSSAO, TexCoords).r;

	vec3 lightResult = vec3(0.0, 0.0, 0.0);
	for (int i = 0; i < pointLightCount; i++) {
		lightResult += CalcPointLight(pointLight[i], fragPos, normal, specular, ao);
	}

	frag_color = vec4(texture(gAlbedoSpec, TexCoords).rgb, 1.0) * vec4(lightResult, 1.0);
}
