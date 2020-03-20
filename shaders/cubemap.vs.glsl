#version 330 core

layout (location = 0) in vec3 in_position;

out vec3 TexCoords;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main()
{
	gl_Position = (projectionMatrix * viewMatrix * vec4(in_position, 1.0)).xyww;
	TexCoords = in_position;
}
