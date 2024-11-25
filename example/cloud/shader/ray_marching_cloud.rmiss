
#version 460
#extension GL_EXT_ray_tracing : require

#extension GL_EXT_debug_printf : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_ARB_shader_clock : enable
#include "Binding.h"
#include "Push_Constants.h"
#include "hit_payload.glsl"
#include "shaders/Data_struct.h"
layout(location = 1) rayPayloadInEXT CloudPayload cloud_prd;
layout(location = 2) rayPayloadEXT light_payload light_prd;
layout(push_constant) uniform _PushContant
{
    PushContant_Cloud pcRay;
};

layout(set = e_ray_tracing, binding = e_tlas) uniform accelerationStructureEXT topLevelAS;
// layout(set = 0, binding = e_noise_data, rgba32f) uniform image3D noise;
layout(set = 0, binding = e_noise_data) uniform sampler3D noise;
float lightAbsorptionTowardSun = 1.f;
float lightAbsorptionThroughCloud = 1.f;

void main()
{
    // debugPrintfEXT("message \n");

    float one_step = .1f;

    vec3 cur_world_pos = cloud_prd.ray_origin + cloud_prd.ray_dir * one_step;
    // vec3 sun_pos = pcRay.lightPosition.xyz;
    ivec3 cur_uvw = ivec3(cur_world_pos * 128) % 128;

    // float cur_density = imageLoad(noise, cur_uvw).x;
    vec3 sun_pos = pcRay.lightPosition.xyz;

    light_prd.steps = 0;
    light_prd.total_density = 0;
    light_prd.ray_origin = cur_world_pos;
    light_prd.ray_dir = normalize(sun_pos - cur_world_pos);

    traceRayEXT(topLevelAS, // acceleration structure
                gl_RayFlagsNoneEXT, // rayFlags
                0xFF, // cullMask
                1, // sbtRecordOffset
                0, // sbtRecordStride
                2, // missIndex
                light_prd.ray_origin, // ray origin
                0.01f, // ray min range
                light_prd.ray_dir, // ray direction
                one_step, // ray max range
                2 // payload (location = 2 for light marching)
    );

    float transmittance = exp(-light_prd.total_density * lightAbsorptionTowardSun);

    if (cloud_prd.depth < 10) {
        // debugPrintfEXT("message %f %f %f %d \n", prd.ray_origin.x,prd.ray_origin.y,prd.ray_origin.z ,prd.depth);

        cloud_prd.ray_origin = cur_world_pos;

        uint rayFlags = gl_RayFlagsNoneEXT;
        cloud_prd.depth++;
        // ivec3 cur_uvw = ivec3(cur_world_pos * 32 + pcRay.offset) % 128;
        // uint r = uint(clockARB());
        // debugPrintfEXT("message %d\n", r);
        // cloud_prd.total_density += pow(imageLoad(noise, cur_uvw).r, 5);
        {
            vec3 cur_sim = cur_world_pos * pcRay.noise_scale + float(pcRay.offset) * pcRay.speed / 10000.f;
            cloud_prd.total_density += transmittance * pow(texture(noise, cur_sim).r, 5);
        }

        // debugPrintfEXT("message %f\n", cloud_prd.total_density);
        // debugPrintfEXT("message %d\n", cloud_prd.depth);

        traceRayEXT(topLevelAS, // acceleration structure
                    rayFlags, // rayFlags
                    0xFF, // cullMask
                    1, // sbtRecordOffset
                    0, // sbtRecordStride
                    1, // missIndex
                    cloud_prd.ray_origin, // ray origin
                    0.1, // ray min range
                    cloud_prd.ray_dir, // ray direction
                    one_step, // ray max range
                    1 // payload (location = 1)
        );
    }
}