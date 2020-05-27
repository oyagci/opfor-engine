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
