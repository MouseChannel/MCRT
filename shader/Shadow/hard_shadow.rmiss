
#version 460
#extension GL_EXT_ray_tracing : require

#extension GL_EXT_debug_printf : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#include "../Data_struct.h"
// #include "common.h"

layout(location = 0) rayPayloadInEXT hitPayload prd;
void main()
{
    // prd.hitValue = vec3(0.1, 0.1, 0.1);
    prd.miss = true;
    debugPrintfEXT("message  from miss\n");

    // prd.depth = 100;
}