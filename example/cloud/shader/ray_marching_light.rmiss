
#version 460
#extension GL_EXT_ray_tracing : require

#extension GL_EXT_debug_printf : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#include "Binding.h"
#include "Push_Constants.h"
#include "hit_payload.glsl"
#include "shaders/Data_struct.h"
layout(location = 1) rayPayloadInEXT light_payload prd;
layout(push_constant) uniform _PushContant
{
    PushContant_Cloud pcRay;
};
layout(set = e_ray_tracing, binding = e_tlas) uniform accelerationStructureEXT topLevelAS;
layout(set = 0, binding = e_noise_data) uniform sampler3D noise;
void main()
{

    float one_step = 1.f;

    vec3 cur_world_pos = prd.ray_origin + one_step;
    // vec3 sun_pos = pcRay.lightPosition.xyz;
    // ivec3 cur_uvw = ivec3(cur_world_pos * 128) % 128;

    float cur_density = texture(noise, cur_world_pos * pcRay.noise_scale).r;
    prd.total_density += pow(cur_density, 5);

    if (prd.steps < 10) {
        // debugPrintfEXT("message %f %f %f %d \n", prd.ray_origin.x,prd.ray_origin.y,prd.ray_origin.z ,prd.depth);

        prd.steps++;
        prd.ray_origin = cur_world_pos;
        uint rayFlags = gl_RayFlagsNoneEXT;
        traceRayEXT(topLevelAS, // acceleration structure
                    rayFlags, // rayFlags
                    0xFF, // cullMask
                    1, // sbtRecordOffset
                    0, // sbtRecordStride
                    2, // missIndex
                    prd.ray_origin, // ray origin
                    0.1, // ray min range
                    prd.ray_dir, // ray direction
                    one_step, // ray max range
                    1 // payload (location = 1 for light payload)
        );
    }
}