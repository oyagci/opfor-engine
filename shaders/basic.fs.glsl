#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 frag_color;

uniform vec3 viewPos;
uniform sampler2D tex;

void main()
{
	vec3 light_color = vec3(1.0, 1.0, 0.9);
	vec3 light_dir = -normalize(vec3(-1.0f, -1.0f, -1.0f));
	vec3 normal = normalize(Normal);

	float ambient_strength = 0.1;
	vec3 ambient_light = ambient_strength * light_color;

	float diffuse_strength = max(dot(normal, light_dir), 0.0);
	vec3 diffuse_light = diffuse_strength * light_color;

	vec3 view_dir = normalize(viewPos - FragPos);
	vec3 reflect_dir = normalize(reflect(-light_dir, normal));
	float specular_strength = 0.5;
	float specular = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
	vec3 specular_light = specular_strength * specular * light_color;

	vec4 light = vec4(ambient_light + diffuse_light + specular_light, 1.0);

	frag_color = texture(tex, TexCoords) * light;
}
