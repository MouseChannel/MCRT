
#version 460
#extension GL_EXT_ray_tracing : require

#extension GL_EXT_debug_printf : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#include "shader/Data_struct.h"
#include "Binding.h"
#include "hit_payload.glsl"

layout(location = 0) rayPayloadInEXT hitPayload prd;
layout(set = 0, binding = e_rt_skybox) uniform samplerCube samplerCubeMap;

void main()
{

    prd.hitValue = texture(samplerCubeMap, normalize(prd.ray_dir)).rgb;
    prd.depth = 999;
}