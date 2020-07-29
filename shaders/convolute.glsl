#vertex
#version 330 core

layout (location = 0) in vec3 in_position;

out vec3 localPos;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main()
{
	gl_Position = (projectionMatrix * viewMatrix * vec4(in_position, 1.0));
	localPos = in_position;
}

#fragment
#version 330 core

out vec4 frag_color;

in vec3 localPos;
uniform samplerCube cubemap;

const float PI = 3.14159265359;

void main()
{
	vec3 normal = normalize(localPos);

	vec3 irradiance = vec3(0.0);

	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = cross(up, normal);
	up = cross(normal, right);

	float sampleDelta = 0.025;
	int nrSamples = 0;
	for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

			irradiance += texture(cubemap, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}
	irradiance = PI * irradiance * (1.0 / float(nrSamples));

	frag_color = vec4(irradiance, 1.0);
}
