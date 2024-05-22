
#version 460
#extension GL_GOOGLE_include_directive : enable
#include "Binding.h"
layout(binding = e_skybox) uniform samplerCube samplerCubeMap;

layout(location = 0) in vec3 inUVW;
layout(location = 0) out vec4 outFragColor;

void main()
{

    outFragColor = pow(texture(samplerCubeMap, normalize(inUVW)), vec4(1. / 1));
}