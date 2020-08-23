#vertex
#version 450 core

layout (location = 0) in vec3 in_position;

out vec3 localPos;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main()
{
	localPos = in_position;
	gl_Position = projectionMatrix * viewMatrix * vec4(localPos, 1.0);
}

#fragment
#version 450 core

out vec4 frag_color;
in vec3 localPos;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invAtan;
	uv += 0.5;

	return uv;
}

void main()
{
	vec2 uv = SampleSphericalMap(normalize(localPos));
	vec3 color = texture(equirectangularMap, uv).rgb;
	frag_color = vec4(color, 1.0);
}
