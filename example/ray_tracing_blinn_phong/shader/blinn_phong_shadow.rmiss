#version 460
#extension GL_EXT_ray_tracing : require

#extension GL_EXT_debug_printf : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#include "../Data_struct.h"
#include "hit_payload.glsl"

layout(location = 1) rayPayloadInEXT bool isShadowed;
void main()
{
    // debugPrintfEXT("message from shadow \n");
    isShadowed = false;
}