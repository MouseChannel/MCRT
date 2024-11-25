
#version 450
// #extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_debug_printf : enable
#include "Binding.h"

layout(binding = e_skybox) uniform samplerCube samplerCubeMap;

// layout(binding = 2) uniform samplerCube samplerCubeMap;

layout(location = 0) in vec3 inUVW;
layout(location = e_gbuffer_color_renderTarget) out vec4 outFragColor;

void main()
{
    // outFragColor = vec4(1, 0, 0, 1);

    outFragColor = texture(samplerCubeMap, normalize(inUVW));
    // debugPrintfEXT("message 12 \n");
}