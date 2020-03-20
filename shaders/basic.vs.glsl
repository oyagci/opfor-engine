#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 2) in vec2 tex_coords;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 viewProjectionMatrix;
uniform mat4 modelMatrix;

out vec3 FragPos;
out vec2 TexCoords;

void main()
{
	gl_Position = viewProjectionMatrix * modelMatrix * vec4(in_position, 1.0);
	FragPos = vec3(modelMatrix * vec4(in_position, 1.0));
	TexCoords = tex_coords;
}
