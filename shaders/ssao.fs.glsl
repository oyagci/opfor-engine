#version 450 core

out float frag_color;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform mat4 projectionMatrix;
uniform vec3 samples[64];

const float noiseSize = 4.0;
const vec2 screenSize = { 2560.0, 1440.0 };
const vec2 noiseScale = vec2(screenSize.x / noiseSize, screenSize.y / noiseSize);
const float bias = 0.025;
const int kernelSize = 64;
const float radius = 0.5;

void main()
{
	vec3 fragPos = texture(gPosition, TexCoords).xyz;
	vec3 normal = normalize(texture(gNormal, TexCoords).xyz);
	vec3 randVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);

	vec3 tangent = normalize(randVec - normal * dot(randVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	for (int i = 0; i < kernelSize; i++) {
		vec3 _sample = TBN * samples[i];
		_sample = fragPos + _sample * radius;

		vec4 off = vec4(_sample, 1.0);
		off = projectionMatrix * off;
		off.xyz /= off.w;
		off.xyz = off.xyz * 0.5 + 0.5;

		float sampleDepth = texture(gPosition, off.xy).z;
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= _sample.z + bias ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1.0 - (occlusion / kernelSize);
	frag_color = occlusion;
}
