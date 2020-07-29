#vertex
#version 450 core

layout (location = 0) in vec3 in_position;
layout (location = 2) in vec2 tex_coords;

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;

out vec2 TexCoords;

void main()
{
	gl_Position = projectionMatrix * modelMatrix * vec4(in_position.xy, 0.0f, 1.0f);
	TexCoords = tex_coords;
}

#fragment
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
