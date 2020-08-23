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

out float frag_color;

in vec2 TexCoords;

uniform sampler2D ssaoInput;

void main()
{
	vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
	float result = 0.0;
	for (int x = -2; x < 2; x++) {
		for (int y = -2; y < 2; y++) {
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture(ssaoInput, TexCoords + offset).r;
		}
	}
	frag_color = result / (4.0 * 4.0);
}
