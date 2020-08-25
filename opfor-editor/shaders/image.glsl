#vertex
#version 450 core

vec2 quad[4] = {
	{ -1.0, -1.0 },
	{  1.0, -1.0 },
	{  1.0,  1.0 },
	{ -1.0,  1.0 },
};

vec2 texCoords[4] = {
	{ 0.0, 0.0 },
	{ 1.0, 0.0 },
	{ 1.0, 1.0 },
	{ 0.0, 1.0 },
};

out vec2 TexCoords;

void main()
{
	gl_Position = vec4(quad[gl_VertexID], 0.0, 1.0);
	TexCoords = texCoords[gl_VertexID];
}

#fragment
#version 450 core

out vec4 frag_color;
in vec2 TexCoords;

uniform sampler2D image;

void main()
{
	frag_color = vec4(texture(image, TexCoords).rgb, 1.0);
}