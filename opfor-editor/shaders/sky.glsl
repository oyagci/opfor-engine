#compute
#version 460 core

#define FLT_MAX 3.402823466e+38
#define FLT_MIN 1.175494351e-38
#define DBL_MAX 1.7976931348623158e+308
#define DBL_MIN 2.2250738585072014e-308

// Compute Shader

layout (local_size_x = 1, local_size_y = 1) in;
layout (rgba16f, binding = 0) uniform image2D img_output;

uniform vec3 viewPos;
uniform vec3 viewDir;
uniform mat4 inverseProjectionMatrix;
uniform mat4 inverseViewMatrix;

uniform sampler2D gColor;
uniform sampler2D gDepth;
uniform sampler2D gPosition;

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
	float viewport_height = 2.0 * h;
	float viewport_width = viewport_height * aspectRatio;
	
	vec3 w = normalize(lookFrom - lookAt);
	vec3 u = normalize(cross(vup, w));
	vec3 v = cross(w, u);

	c.origin = lookFrom;
	c.horizontal = viewport_width * u;
	c.vertical = viewport_height * v;
	c.lower_left_corner = c.origin - c.horizontal / 2.0 - c.vertical / 2.0 - w;

	return c;
}

int numInScatteringPoints = 10;
int numOpticalScatteringPoints = 10;
float atmosphereRadius = 120.0;
vec3 planetCentre = { 0.0, 0.0, 0.0 };
float planetRadius = 100.0;
vec3 sunPosition = { 0.0, 0.0, -1000.0 };
float densityFalloff = 8.0;

float densityAtPoint(vec3 densitySamplePoint)
{
	float heightAboveSurface = length(densitySamplePoint - planetCentre) - planetRadius;
	float height01 = heightAboveSurface / (atmosphereRadius - planetRadius);
	float localDensity = exp(-height01 * densityFalloff) * (1.0 - height01);

	return localDensity;
}

float opticalLength(vec3 rayOrigin, vec3 rayDir, float rayLength)
{
	vec3 densitySamplePoint = rayOrigin;
	float stepSize = rayLength / (numOpticalScatteringPoints - 1);
	float opticalDepth = 0.0;

	for (int i = 0; i < numOpticalScatteringPoints; i++) {
	
		float localDensity = densityAtPoint(densitySamplePoint);
		opticalDepth += localDensity * stepSize;
		densitySamplePoint += rayDir * stepSize;
	}

	return opticalDepth;
}

float CalcLight(vec3 rayOrigin, vec3 rayDir, float rayLen)
{
	vec3 inScatterPoint = rayOrigin;
	float stepSize = rayLen / (numInScatteringPoints - 1.0);
	float inScatteredLight = 0.0;

	vec3 dirToSun = normalize(-sunPosition);

	for (int i = 0; i < numInScatteringPoints; i++) {
	
		float sunRayLength = raySphere(planetCentre, atmosphereRadius, inScatterPoint, dirToSun).y;
		float sunRayOpticalLength = opticalLength(inScatterPoint, dirToSun, sunRayLength);
		float viewRayOpticalLength = opticalLength(inScatterPoint, -rayDir, stepSize * i);
		float transmittance = exp(-(sunRayOpticalLength + -viewRayOpticalLength));
		float localDensity = densityAtPoint(inScatterPoint);

		inScatteredLight += localDensity * transmittance * stepSize;
		inScatterPoint += rayDir * stepSize;
	}

	return inScatteredLight;
}

float LinearizeDepth(float depth)
{
	float near = 1.0;
	float far = 1000000.0;
	float ndc = depth * 2.0 - 1.0;

	return (2.0 * near * far) / (far + near - ndc * (far - near));
}

vec3 ViewToWorldSpace(vec3 viewSpacePosition)
{
	vec4 worldSpace = inverseProjectionMatrix * vec4(viewSpacePosition, -1.0);	
	worldSpace = inverseViewMatrix * vec4(viewSpacePosition, 0.0);

	return worldSpace.xyz;
}

void main()
{
	vec2 dimensions = imageSize(img_output);
	vec2 uv = vec2(gl_GlobalInvocationID.x / dimensions.x, gl_GlobalInvocationID.y / dimensions.y);	
	vec3 viewVector = ViewToWorldSpace(vec3(uv * 2.0 - 1.1, 0.0));
	//vec3 viewVector = ViewToWorldSpace(texture(gPosition, uv).rgb);
	float aspectRatio = dimensions.x / dimensions.y;

	vec4 originalColor = texture(gColor, uv);
	float sceneDepthNonLinear = texture(gDepth, uv).r;
	float sceneDepth = LinearizeDepth(sceneDepthNonLinear);// * length(viewVector);

	vec3 rayOrigin = viewPos;
	vec3 rayDir = normalize(viewDir);

	vec2 hitInfo = raySphere(planetCentre, atmosphereRadius, rayOrigin, rayDir);

	float dstToAtmosphere = hitInfo.x;
	float dstThroughAtmosphere = min(hitInfo.y, sceneDepth - dstToAtmosphere);

	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	float color = dstThroughAtmosphere / (atmosphereRadius / 2.0);
	imageStore(img_output, pixel_coords, vec4(color.rrr, 1.0));
	
	//vec4 color = originalColor;
	//
	//if (dstThroughAtmosphere > 0.0) {
	//	const float epsilon = 0.0001;
	//	vec3 pointInAtmosphere = rayOrigin + rayDir * (dstToAtmosphere + epsilon);
	//	float light = CalcLight(pointInAtmosphere, rayDir, dstThroughAtmosphere - epsilon * 2.0);
	//
	//	color = originalColor * (1.0 - light) + light;
	//}
	//
	//ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	//imageStore(img_output, pixel_coords, color);
}	