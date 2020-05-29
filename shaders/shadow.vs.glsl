#version 450 core

layout (location = 0) in vec3 in_position;

uniform mat4 modelMatrix;

void main()
{
	gl_Position = modelMatrix * vec4(in_position, 1.0);
}
