#version 450 core

out vec4 frag_color;

in vec2 TexCoords;

uniform sampler2D diffuse;

void main()
{
	frag_color = texture(diffuse, TexCoords);
}
