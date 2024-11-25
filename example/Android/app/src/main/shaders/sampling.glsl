#ifdef __cplusplus
#include "Data_struct.h"
#endif
#ifndef PI
#define PI 3.1415926
#endif

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