#version 450 core

out vec4 frag_color;

in vec2 TexCoords;

uniform sampler2D tex;
uniform vec4 color;

void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(tex, TexCoords).r);
	if (color.a > 0.0) {
		frag_color = color * sampled;
	}
	else {
		frag_color = texture(tex, TexCoords);
	}
}
