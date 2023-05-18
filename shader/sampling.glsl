#ifdef __cplusplus
#include "Data_struct.h"
#endif

// Generate a random unsigned int from two unsigned int values, using 16 pairs
// of rounds of the Tiny Encryption Algorithm. See Zafar, Olano, and Curtis,
/*
// "GPU Random Numbers via the Tiny Encryption Algorithm"
uint tea(uint val0, uint val1)
{
    uint v0 = val0;
    uint v1 = val1;
    uint s0 = 0;

    for (uint n = 0; n < 16; n++) {
        s0 += 0x9e3779b9;
        v0 += ((v1 << 4) + 0xa341316c) ^ (v1 + s0) ^ ((v1 >> 5) + 0xc8013ea4);
        v1 += ((v0 << 4) + 0xad90777d) ^ (v0 + s0) ^ ((v0 >> 5) + 0x7e95761e);
    }

    return v0;
}
// Generate a random unsigned int in [0, 2^24) given the previous RNG state
// using the Numerical Recipes linear congruential generator
uint lcg(inout uint prev)
{
    uint LCG_A = 1664525u;
    uint LCG_C = 1013904223u;
    prev = (LCG_A * prev + LCG_C);
    return prev & 0x00FFFFFF;
}

// Generate a random float in [0, 1) given the previous RNG state
float rnd(inout uint prev)
{
    return (float(lcg(prev)) / float(0x01000000));
}
*/

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
    float cosTheta = sqrt(u);
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    // float theta = asin(sqrt(u));
    // return vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));

    return vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
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
