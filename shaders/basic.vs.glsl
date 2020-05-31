#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 tex_coords;
layout (location = 3) in vec3 in_tangent;
layout (location = 4) in float in_texture;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 viewProjectionMatrix;
uniform mat4 modelMatrix;
uniform vec3 viewPos;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out mat3 TBN;
out float DiffuseTex;

void main()
{
	gl_Position = viewProjectionMatrix * modelMatrix * vec4(in_position, 1.0);
	FragPos = vec3(modelMatrix * vec4(in_position, 1.0));
	TexCoords = tex_coords;
	Normal = mat3(transpose(inverse(modelMatrix))) * in_normal;
	DiffuseTex = in_texture;

	vec3 T = normalize(vec3(modelMatrix * vec4(in_tangent, 0.0)));
	vec3 N = normalize(vec3(modelMatrix * vec4(in_normal, 0.0)));
	vec3 B = normalize(cross(N, T));
	TBN = mat3(T, B, N);
}
