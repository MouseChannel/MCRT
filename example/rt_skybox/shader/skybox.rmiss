
#version 460
#extension GL_EXT_ray_tracing : require

#extension GL_EXT_debug_printf : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_ARB_shader_clock : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#include "../Data_struct.h"
#include "Binding.h"
#include "hit_payload.glsl"

layout(location = 0) rayPayloadInEXT hitPayload prd;
layout(set = 0, binding = e_skybox, rgba8) uniform imageCube samplerCubeMap;
// layout(set = 0, binding = e_skybox) uniform samplerCube samplerCubeMap;

void main()
{
    float m_x = abs(prd.ray_dir.x);
    float m_y = abs(prd.ray_dir.y);
    float m_z = abs(prd.ray_dir.z);
    int size = 2048;
    ivec3 fixed_texcoord = ivec3(size) - ivec3((prd.ray_dir + 1.) / 2 * size);
    // fixed_texcoord = ivec3((prd.ray_dir + 1.) / 2 * size);
    // if (gl_LaunchIDEXT.x == 0 && gl_LaunchIDEXT.y == 0) {
    //     debugPrintfEXT("message %d %d %d %d\n", fixed_texcoord.x, fixed_texcoord.y, fixed_texcoord.z, uint(clockARB()));
    //     debugPrintfEXT("message %f %f %f \n", prd.ray_dir.x, prd.ray_dir.y, prd.ray_dir.z);
    // }
    // ivec3 fixed_texcoord = ivec3(prd.ray_dir * size);
    // if (prd.ray_dir.x < 0)
    // debugPrintfEXT("message %d %d %d \n", fixed_texcoord.x, fixed_texcoord.y, fixed_texcoord.z);
    // debugPrintfEXT("message %f %f %f \n", prd.ray_dir.x, prd.ray_dir.y, prd.ray_dir.z);
    int z = -1;
    vec4 c = vec4(1, 0, 0, 1);
    float m = max(max(m_x, m_y), m_z);
    if (m - m_x < 1e-10) {
        if (prd.ray_dir.x > 0) {
            z = 0;
        } else {
            z = 1;
        }
        c = imageLoad(samplerCubeMap, ivec3(fixed_texcoord.z, fixed_texcoord.y, z));
    }
    if (m - m_y < 1e-10) {
        if (prd.ray_dir.y > 0) {
            z = 2;
        } else {
            z = 3;
        }
        c = imageLoad(samplerCubeMap, ivec3(fixed_texcoord.x, fixed_texcoord.z, z));
    }
    if (m - m_z < 1e-10) {
        if (prd.ray_dir.z > 0) {
            z = 4;
        } else {
            z = 5;
        }
        vec2 temp = vec2(fixed_texcoord.x, fixed_texcoord.y);
        c = imageLoad(samplerCubeMap, ivec3(temp, z));
        if (gl_LaunchIDEXT.x == 0 && gl_LaunchIDEXT.y == 400) {
            debugPrintfEXT("message %d %d %d %d\n", fixed_texcoord.x, fixed_texcoord.y, fixed_texcoord.z, uint(clockARB()));
        }
    }

    // debugPrintfEXT("message %f %f %f \n", c.x, c.y, c.z);
    // debugPrintfEXT("message color = %f %f %f ||uv =  %d %d %d \n", c.x, c.y, c.z, uu.x, uu.y, uu.z);
    // c = vec4(1, 0, 0, 1);
    prd.hitValue = c.xyz;

    // prd.hitValue = texture(samplerCubeMap, normalize(prd.ray_dir)).xyz;
    prd.depth = 999;
}