#compute
#version 460 core

#define FLT_MAX 3.402823466e+38
#define FLT_MIN 1.175494351e-38
#define DBL_MAX 1.7976931348623158e+308
#define DBL_MIN 2.2250738585072014e-308

// Compute Shader

layout (local_size_x = 1, local_size_y = 1) in;
layout (rgba16f, binding = 0) uniform image2D img_output;

uniform mat4 viewProjectionMatrix;
uniform vec3 viewPos;
uniform vec3 viewDir;

struct ray
{
	vec3 origin;
	vec3 dir;
};

vec2 raySphere(vec3 sphereCentre, float sphereRadius, vec3 rayOrigin, vec3 rayDir)
{
	vec3 offset = rayOrigin - sphereCentre;
	float a = dot(rayDir, rayDir);
	float b = 2.0 * dot(offset, rayDir);
	float c = dot(offset, offset) - sphereRadius * sphereRadius;
	float d = b * b - 4.0 * a * c;

	if (d > 0.0) {
		float s = sqrt(d);
		float dstToSphereNear = max(0.0, (-b - s) / (2.0 * a));
		float dstToSphereFar = (-b + s) / (2.0 * a);

		// Ignore intersections that occur behind the ray
		if (dstToSphereFar >= 0.0) {
			return vec2(dstToSphereNear, dstToSphereFar - dstToSphereNear);
		}
	}

	// Ray did not intersect sphere
	return vec2(FLT_MAX, 0.0);
}

struct Camera
{
	vec3 origin;
	vec3 horizontal;
	vec3 vertical;
	vec3 lower_left_corner;
};

struct Sphere
{
	vec3 position;
	float radius;
};

ray get_ray(Camera c, float s, float t)
{
	ray r = { c.origin, c.lower_left_corner + s * c.horizontal + t * c.vertical - c.origin };

	return r;
}

vec2 castRay(Camera c, Sphere s)
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec2 dimensions = imageSize(img_output);

	float u = float(pixel_coords.x * 1.0) / dimensions.x;
	float v = float(pixel_coords.y * 1.0) / dimensions.y;

	ray r = get_ray(c, u, v);

	vec2 hitInfo = raySphere(s.position, s.radius, r.origin, r.dir);

	return hitInfo;
}

Camera make_camera(vec3 lookFrom, vec3 lookAt, vec3 vup, float hfov, float aspectRatio)
{
	Camera c;

	float theta = radians(hfov);
	float h = theta / 2.0;
	float viewport_width = 2.0 * h;
	float viewport_height = aspectRatio / viewport_width;
	
	vec3 w = normalize(lookFrom - lookAt);
	vec3 u = normalize(cross(vup, w));
	vec3 v = cross(w, u);

	c.origin = lookFrom;
	c.horizontal = viewport_width * u;
	c.vertical = viewport_height * v;
	c.lower_left_corner = c.origin - c.horizontal / 2.0 - c.vertical / 2.0 - w;

	return c;
}

void main()
{
	vec2 dimensions = imageSize(img_output);
	float aspectRatio = dimensions.x / dimensions.y;

	Camera c = make_camera(viewPos, viewPos + viewDir, vec3(0, 1, 0), 90.0, aspectRatio);
	Sphere s = { vec3(0, 0, 0), 100.0 };

	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

	vec2 hitInfo = castRay(c, s);

	float atmosphereRadius = 110.0;

	float dstToAtmosphere = hitInfo.x;
	float dstThroughAtmosphere = hitInfo.y;

	float color = dstThroughAtmosphere / (atmosphereRadius * 2.0);

	imageStore(img_output, pixel_coords, vec4(color.rrr, 1.0));
}