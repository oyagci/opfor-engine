#version 450 core

out vec4 frag_color;

in vec2 TexCoords;

uniform sampler2D screen_texture;

void main()
{
	frag_color = texture(screen_texture, TexCoords);
}
