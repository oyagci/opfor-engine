#version 450 core
#define MAX_NUM_POINT_LIGHTS		16
#define MAX_NUM_DIRECTIONAL_LIGHTS	1

struct PointLight {
	vec3 position;
	vec3 color;
	float intensity;
	float radius;
};

struct DirectionalLight {
	vec3 direction;
	vec3 color;
	float intensity;
};

out vec4 frag_color;

uniform vec3 viewPos;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gSSAO;
uniform sampler2D gMetallicRoughness;
uniform samplerCube depthMap;

uniform float exposure;
uniform PointLight pointLight[MAX_NUM_POINT_LIGHTS];
uniform int pointLightCount;

uniform DirectionalLight directionalLights[MAX_NUM_DIRECTIONAL_LIGHTS];
uniform int directionalLightCount;

in vec2 TexCoords;

float CalcShadow(vec3 lightPos, vec3 fragPos)
{
	float far_plane = 10000.0;
	vec3 fragToLight = fragPos - lightPos;
	float currentDepth = length(fragToLight);
	float bias = 2.3;
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

const float PI = 3.14159265359;

// Normal Distribution Function
float DistributionTrowbridgeReitzGGX(vec3 normal, vec3 halfway, float roughness)
{
	float a = roughness * roughness;
	float aa = a * a;

	float NdotH = max(dot(normal, halfway), 0.0);
	float NdotH2 = NdotH * NdotH;

	float denom = PI * pow(NdotH2 * (aa - 1.0) + 1.0, 2.0);

	return aa / denom;
}

// Geometry Function
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0;

	return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);

	return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1 - cosTheta, 5.0);
}

vec3 CalcPbr()
{
	vec3 fragPos = texture(gPosition, TexCoords).rgb;
	vec3 fragColor = texture(gAlbedoSpec, TexCoords).rgb;

	vec3 N = texture(gNormal, TexCoords).rgb;
	vec3 V = normalize(viewPos - fragPos).rgb;

	const float metallicFactor = texture(gMetallicRoughness, TexCoords).b;
	const float roughnessFactor = texture(gMetallicRoughness, TexCoords).g;

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, fragColor, metallicFactor);

	vec3 Lo = vec3(0.0);
	for (int i = 0; i < pointLightCount; i++) {
		vec3 L = normalize(pointLight[i].position - fragPos);
		vec3 H = normalize(V + L);

		float dist = length(pointLight[i].position - fragPos);
		float attenuation = 1.0 / (dist * dist);
		vec3 radiance = pointLight[i].color * pointLight[i].intensity * attenuation;

		// DFG

		float NDF = DistributionTrowbridgeReitzGGX(N, H, roughnessFactor);
		float G = GeometrySmith(N, V, L, roughnessFactor);
		vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 ks = F;
		vec3 kd = vec3(1.0) - ks;
		kd *= 1.0 - metallicFactor;

		vec3 DFG = NDF * G * F;
		float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		vec3 specular = DFG / max(denom, 0.001);

		float NdotL = max(dot(N, L), 0.0);
		Lo += ((kd * fragColor / PI + specular) * radiance * NdotL);
	}

	//float shadow = CalcShadow(pointLight[0].position, fragPos);
	float shadow = 0.0;

	vec3 ambient = vec3(0.03) * fragColor;
	vec3 color = ambient + Lo;

	color = color * (1.0 - shadow);

	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0 / 2.2));

	return color;
}

void main()
{
	vec3 color = CalcPbr();

	color = vec3(1.0) - exp(-color * exposure);

	frag_color = vec4(color, 1.0);
}
