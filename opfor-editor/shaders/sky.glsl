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

uniform int   numInScatteringPoints;
uniform int   numOpticalScatteringPoints;
uniform float atmosphereRadius;
uniform vec3  planetCenter;
uniform float planetRadius;
uniform vec3  sunPosition;
uniform float densityFalloff;
uniform vec3 scatteringCoefficients;

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
	worldSpace = inverseViewMatrix * vec4(worldSpace.rgb, 0.0);

	return worldSpace.xyz;
}

vec2 raySphere(vec3 sphereCentre, float sphereRadius, vec3 rayOrigin, vec3 rayDir)
{
	vec3 offset = rayOrigin - sphereCentre;
	float a = 1.0;
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

float densityAtPoint(vec3 densitySamplePoint)
{
	float heightAboveSurface = length(densitySamplePoint - planetCenter) - planetRadius;
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

vec3 calculateLight(vec3 rayOrigin, vec3 rayDir, float rayLen, vec3 originalColor)
{
	vec3 inScatterPoint = rayOrigin;
	float stepSize = rayLen / (numInScatteringPoints - 1);
	vec3 inScatteredLight = vec3(0.0, 0.0, 0.0);
	float viewRayOpticalDepth = 0.0;

	vec3 dirToSun = normalize(-sunPosition);

	for (int i = 0; i < numInScatteringPoints; i++)
	{
		float sunRayLength = raySphere(planetCenter, atmosphereRadius, inScatterPoint, dirToSun).y;
		float sunRayOpticalDepth = opticalLength(inScatterPoint, dirToSun, sunRayLength);
		viewRayOpticalDepth = opticalLength(inScatterPoint, -rayDir, stepSize * i);
		vec3 transmittance = exp(-(sunRayOpticalDepth + viewRayOpticalDepth) * scatteringCoefficients);
		float localDensity = densityAtPoint(inScatterPoint);

		inScatteredLight += localDensity * transmittance * scatteringCoefficients * stepSize;
		inScatterPoint += rayDir * stepSize;
	}

	float originalColorTransmittance = exp(-viewRayOpticalDepth);

	return originalColor * originalColorTransmittance + inScatteredLight;
}

vec3 getViewVector()
{
	vec2 dimensions = imageSize(img_output);
	vec2 uv = vec2(gl_GlobalInvocationID.x / dimensions.x, gl_GlobalInvocationID.y / dimensions.y);

	vec3 viewVector = (inverseProjectionMatrix * vec4(uv * 2.0 - 1.0, 0.0, 1.0)).rgb;
	viewVector = (inverseViewMatrix * vec4(viewVector, 0.0)).rgb;

	return viewVector;
}

vec2 getUV()
{
	vec2 dimensions = imageSize(img_output);
	vec2 uv = vec2(gl_GlobalInvocationID.x / dimensions.x, gl_GlobalInvocationID.y / dimensions.y);

	return uv;
}

vec4 skyColor()
{
	vec2 uv = getUV();
	vec3 viewVector = getViewVector();

	vec4 originalColor = texture(gColor, uv);
	float sceneDepthNonLinear = texture(gDepth, uv).r;
	float sceneDepth = LinearizeDepth(sceneDepthNonLinear) * length(viewVector);

	vec3 rayOrigin = viewPos;
	vec3 rayDir = normalize(viewVector);

	vec2 hitInfo = raySphere(planetCenter, atmosphereRadius, rayOrigin, rayDir);

	float dstToAtmosphere = hitInfo.x;
	float dstThroughAtmosphere = min(hitInfo.y, sceneDepth - dstToAtmosphere);

	if (dstThroughAtmosphere > 0) {
		const float epsilon = 0.0001;
		vec3 pointInAtmosphere = rayOrigin + rayDir * (dstToAtmosphere - epsilon);
		vec3 light = calculateLight(pointInAtmosphere, rayDir, (dstThroughAtmosphere + epsilon * 2), originalColor.rgb);
				
		return vec4(light, 0.0);
	}

	return originalColor;
}

void main()
{
	vec4 color = skyColor();

	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	imageStore(img_output, pixel_coords, color);
}	