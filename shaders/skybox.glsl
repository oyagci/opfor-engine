#vertex
#version 330 core

layout (location = 0) in vec3 in_position;

out vec3 localPos;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main()
{
	gl_Position = (projectionMatrix * viewMatrix * vec4(in_position, 1.0)).xyww;
	localPos = in_position;
}

#fragment
#version 330 core

out vec4 frag_color;

in vec3 localPos;
uniform samplerCube cubemap;

void main()
{
	vec3 color = texture(cubemap, localPos).rgb;

	color = color / (color + vec3(1.0));
	//color = pow(color, vec3(1.0 / 2.2));

	frag_color = vec4(color, 1.0);
}
