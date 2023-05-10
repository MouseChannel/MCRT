#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_debug_printf : enable

#include "common.glsl"

hitAttributeEXT vec2 attribs;

layout(location = 0) rayPayloadInEXT hitPayload prd;
void main()
{

    prd.hitValue = vec3(1, 0, 1);
    gl_LaunchIDEXT;
    gl_WorldRayOriginEXT;
    gl_WorldToObjectEXT;
    // debugPrintfEXT("message im in clost hit shader \n");
    // gl_fr
}