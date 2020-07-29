#vertex
#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 2) in vec2 tex_coords;

uniform mat4 projectionMatrix;

out vec2 TexCoords;

void main()
{
	gl_Position = projectionMatrix * vec4(in_position.xy, 0.0f, 1.0f);
	TexCoords = tex_coords;
}

#fragment
#version 330 core

in vec2 TexCoords;

out vec4 frag_color;

uniform sampler2D tex;

void main()
{
	frag_color = texture(tex, TexCoords);
}
