#version 460
#extension GL_EXT_ray_tracing : require

#extension GL_EXT_debug_printf : enable
#extension GL_GOOGLE_include_directive : enable

#include "common.glsl"
#include "common.h"

layout(location = 0) rayPayloadInEXT hitPayload prd;
void main()
{
    prd.hitValue = vec3(0, 1, 0);

    // gl_launchId
    // gl_LaunchIDEXT;
    // gl_fr`
}