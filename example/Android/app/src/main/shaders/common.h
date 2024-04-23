
#ifdef __cplusplus
#pragma once
#include "glm/glm.hpp"
#include "vulkan/vulkan.hpp"
using mat4 = glm::mat4;
using vec4 = glm::vec4;

using vec3 = glm::vec3;
using vec2 = glm::vec2;
using ivec3 = glm::ivec3;
// using uint63 = unsigned long;
using mat4x3 = glm::mat4x3;
using mat3 = glm::mat3;
using uint = unsigned int;
using namespace glm;
#endif
#ifndef PI
#define PI 3.1415926
#endif
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
vec3 F_SchlickR(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

//[Walter 2007]
float D_GGX(float NoH, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float NoH2 = NoH * NoH;
    float b = NoH2 * (alpha2 - 1.0) + 1.0;
    return alpha2 / (PI * b * b);
}
// TODO [Burley 2012]
//  float D_GTR(float NoH, float roughness)
//  {
//      float alpha = roughness * roughness;
//      float alpha2 = alpha * alpha;
//      float NoH2 = NoH * NoH;
//      float b = (NoH2 * (alpha2 - 1.0) + 1.0);
//      return alpha2 / (PI * b * b);
//  }
float G1_GGX_Schlick(float NdotV, float roughness)
{
    // float r = roughness; // original
    float r = pow(0.5 + 0.5 * roughness, 2); // Disney remapping
    float k = (r * r) / 2.0;
    float denom = NdotV * (1.0 - k) + k;
    return NdotV / denom;
}

float G_Smith(float NoV, float NoL, float roughness)
{
    float g1_l = G1_GGX_Schlick(NoL, roughness);
    float g1_v = G1_GGX_Schlick(NoV, roughness);
    return g1_l * g1_v;
}
// cook-torrance BRDF
// f_r = k_d* f_lambert + k_s* f_cook-torrance
vec3 microfacetBRDF(vec3 L, vec3 V, vec3 N, vec3 baseColor, float metallicness, float roughness)
{

    vec3 H = normalize(V + L); // half vector

    // all required dot products
    float NoV = clamp(dot(N, V), 0.0, 1.0);
    float NoL = clamp(dot(N, L), 0.0, 1.0);
    float NoH = clamp(dot(N, H), 0.0, 1.0);
    float VoH = clamp(dot(V, H), 0.0, 1.0);

    //   cook-torrance

    // vec3 f0 = metallicness * baseColor + (1 - metallicness) * 0.04;
    //
    vec3 f0 = vec3(0.16 * (0.5 * 0.5));
    // in case of metals, baseColor contains F0
    f0 = mix(f0, baseColor, metallicness);
    //
    vec3 F = fresnelSchlick(VoH, f0);
    float D = D_GGX(NoH, roughness);
    float G = G_Smith(NoV, NoL, roughness);
    // if (G < 0.5) {
    //     debugPrintfEXT("message %f %f %f  \n", G, G, G);
    // }
    // return vec3(G);
    vec3 specular = F * G * D / max(4.0 * NoV * NoL, 0.001);
    // lambert
    vec3 diffuse = baseColor / PI;

    vec3 k_s = F;
    vec3 k_d = (vec3(1) - k_s) * (1 - metallicness);

    return k_d * diffuse + k_s * specular;
}
vec3 ImportanceSampleGGX(vec2 xi, vec3 N, float roughness)
{
    float a = roughness * roughness;

    float phi = 2.0 * PI * xi.x;
    float cosTheta = sqrt((1.0 - xi.y) / (1.0 + (a * a - 1.0) * xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sinTheta * sin(phi);
    H.z = cosTheta;

    return normalize(H);
}

vec3 Get_IBLColor(vec3 camera_pos,
                  vec3 light_pos,
                  vec3 fragment_world_pos,
                  vec3 fragment_world_nrm,
                  float metallicness,
                  float roughness,
                  vec3 albedo,
                  samplerCube skybox,
                  samplerCube irradiance_cubemap,
                  sampler2D LUT_image)
{
    vec3 V = normalize(vec3(camera_pos) - fragment_world_pos);
    vec3 L = normalize(vec3(light_pos) - fragment_world_pos);
    vec3 H = normalize(V + L);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallicness);
    // vec3 F = fresnelSchlick(max(dot(V, H), 0.0), F0);
    vec3 F = F_SchlickR(max(dot(fragment_world_nrm, V), 0.0), F0, roughness);
    vec3 KS = F;
    vec3 KD = vec3(1.0) - KS;

    vec3 irradiance = texture(irradiance_cubemap, fragment_world_nrm).rgb;
    vec3 diffuse = albedo * irradiance;
 

    vec3 dir = reflect(-V, fragment_world_nrm);
    vec3 prefilteredColor = textureLod(skybox, dir, roughness * 10).rgb;
   
    // prefilteredColor = texture(skybox, dir).rgb;
    vec3 brdf = texture(LUT_image, vec2(max(dot(fragment_world_nrm, V), 0.0), roughness)).rgb;
     
    vec3 specular = prefilteredColor * (F0 * brdf.r + brdf.g);
return  diffuse + specular *0.1; 
    return KD * diffuse + specular ; 
return specular;
}
