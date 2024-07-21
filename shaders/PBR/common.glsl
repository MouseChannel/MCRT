#ifndef PI
#define PI 3.141592f;
#endif
#define EPSILON 0.00001f;

#include "shaders/sampling.glsl"
// float radicalInverseVdC(uint bits)
// {
//     bits = (bits << 16u) | (bits >> 16u);
//     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
//     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
//     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
//     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
//     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
// }

// // Sample i-th point from Hammersley point set of NumSamples points total.
// vec2 sampleHammersley(uint i, float InvNumSamples)
// {
//     return vec2(i * InvNumSamples, radicalInverseVdC(i));
// }

// vec3 sampleHemisphere(float u1, float u2)
// {
//     const float u1p = sqrt(max(0.0, 1.0 - u1 * u1));
//     return vec3(cos(2 * PI * u2) * u1p, sin(2 * PI * u2) * u1p, u1);
// }

vec3 sampleGGX(float u1, float u2, float roughness)
{
    float alpha = roughness * roughness;

    float cosTheta = sqrt((1.0 - u2) / (1.0 + (alpha * alpha - 1.0) * u2));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    float phi = 2 * PI * u1;

    return vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
}
float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}
float gaSchlickGGX_IBL(float cosLi, float cosLo, float roughness)
{
    float r = roughness;
    float k = (r * r) / 2.0; // Epic suggests using this roughness remapping for IBL lighting.
    return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}
float ndfGGX(float cosLh, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (PI * denom * denom);
}

void computeBasisVectors(const vec3 N, out vec3 S, out vec3 T)
{
    // Branchless select non-degenerate T.
    T = cross(N, vec3(0.0, 1.0, 0.0));

    T = mix(cross(N, vec3(1.0f, 0.0, 0.0)), T, step(0.00001f, dot(T, T)));

    T = normalize(T);
    S = normalize(cross(N, T));
}

// Convert point from tangent/shading space to world space.
vec3 tangentToWorld(const vec3 v, const vec3 N, const vec3 S, const vec3 T)
{
    return S * v.x + T * v.y + N * v.z;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 Get_IBLColor(vec3 camera_pos,
                  vec3 fragment_world_pos,
                  vec3 fragment_world_nrm,
                  float metallicness,
                  float roughness,
                  vec3 albedo,
                  samplerCube skybox,
                  samplerCube irradiance_cubemap,
                  sampler2D LUT_image)
{
    vec3 irradiance = texture(irradiance_cubemap, fragment_world_nrm).rgb;
    vec3 V = normalize(vec3(camera_pos) - fragment_world_pos);

    vec3 F0 = mix(vec3(0.04), albedo, metallicness);
    float cosine = max(dot(fragment_world_nrm, V), 0.0);

    vec3 F = fresnelSchlick(cosine, F0);

    vec3 KD = mix(vec3(1.0) - F, vec3(0.0), metallicness);

    vec3 diffuse = KD * albedo * irradiance;

    vec3 dir = reflect(-V, fragment_world_nrm);

    vec3 prefilteredColor = textureLod(skybox, dir, roughness * textureQueryLevels(skybox)).rgb;

    vec3 brdf = texture(LUT_image, vec2(cosine, roughness)).rgb;

    vec3 specular = prefilteredColor * (F0 * brdf.r + brdf.g);

    return diffuse + specular;
}
