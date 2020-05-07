#version 330 core

in vec3 FragPos;
in vec2 TexCoords;

out vec4 frag_color;

uniform sampler2D tex;

void main()
{
	frag_color = texture(tex, TexCoords);
}
