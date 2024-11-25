#ifdef __cplusplus
#include "Data_struct.h"
#endif
#ifndef PI
#define PI 3.1415926
#endif
#ifndef SAMPLING
#define SAMPLING
vec3 pcg3d_random(uvec3 seed)
{
    seed = seed * 1664525u + 1013904223u;
    seed.x += seed.y * seed.z;
    seed.y += seed.z * seed.x;
    seed.z += seed.x * seed.y;
    seed ^= seed >> 16u;
    seed.x += seed.y * seed.z;
    seed.y += seed.z * seed.x;
    seed.z += seed.x * seed.y;
    // Generate a random float in [0, 1)
    return vec3(seed) / float(0xffffffffu);
}

//-------------------------------------------------------------------------------------------------
// Sampling
//-------------------------------------------------------------------------------------------------
vec3 hemisphereSample_cos(in uvec3 seed)
{

    vec3 pcg3d = pcg3d_random(seed);
    float v = pcg3d.x;
    float u = pcg3d.y;
    float phi = v * 2.0 * PI;
    float cosTheta = sqrt(1. - u);
    // float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    // float theta = asin(sqrt(u));
    // return vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));

    return vec3(cos(phi) * sqrt(u), sin(phi) * sqrt(u), cosTheta);
}

vec3 hemisphereSample_uniform(in uvec3 seed)
{

    vec3 pcg3d = pcg3d_random(seed);
    float v = pcg3d.x;
    float u = pcg3d.y;
    float phi = v * 2.0 * PI;
    float cosTheta = u;
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    return vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}
mat3 getNormalSpace(in vec3 normal)
{
    vec3 someVec = vec3(1.0, 0.0, 0.0);
    float dd = dot(someVec, normal);
    vec3 tangent = vec3(0.0, 1.0, 0.0);
    if (1.0 - abs(dd) > 1e-6) {
        tangent = normalize(cross(someVec, normal));
    }
    vec3 bitangent = cross(normal, tangent);
    return mat3(tangent, bitangent, normal);
}



// Compute Van der Corput radical inverse
// See: http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
float radicalInverse_VdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

// Sample i-th point from Hammersley point set of NumSamples points total.
vec2 sampleHammersley(uint i,float InvNumSamples)
{
	return vec2(i * InvNumSamples, radicalInverse_VdC(i));
}

// Uniformly sample point on a hemisphere.
// Cosine-weighted sampling would be a better fit for Lambertian BRDF but since this
// compute shader runs only once as a pre-processing step performance is not *that* important.
// See: "Physically Based Rendering" 2nd ed., section 13.6.1.
vec3 sampleHemisphere(float u1, float u2)
{
	const float u1p = sqrt(max(0.0, 1.0 - u1*u1));
    const float  pi = 3.1415926; 
	return vec3(cos(2*pi*u2) * u1p, sin(2*pi*u2) * u1p, u1);
}
#endif