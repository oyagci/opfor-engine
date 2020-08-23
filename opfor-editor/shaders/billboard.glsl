#vertex
#version 450 core

layout (location = 0) in vec3 in_position;
layout (location = 2) in vec2 in_tex_coord;

uniform mat4 viewProjectionMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform vec3 particlePosition; // in world-space

out vec2 TexCoords;

void main()
{
	vec3 cameraRight = { viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0] };
	vec3 cameraUp = { viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1] };

	vec3 vertexPos = particlePosition
					 + cameraRight * in_position.x * 3.0
					 + cameraUp * in_position.y * 3.0;

	gl_Position = viewProjectionMatrix * vec4(vertexPos, 1.0f);

//	vec3 vertexPos = particlePosition;
//	gl_Position = viewProjectionMatrix * modelMatrix * vec4(vertexPos, 1.0f);
//	gl_Position /= gl_Position.w;
//	gl_Position.xy += in_position.xy * vec2(-0.1, -0.1 * (16.0f / 9.0f));

	TexCoords = in_tex_coord;
}

#fragment
#version 450 core

out vec4 frag_color;

in vec2 TexCoords;

uniform sampler2D diffuse;

void main()
{
	frag_color = texture(diffuse, TexCoords);
}
