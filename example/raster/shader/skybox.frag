
#version 460
#extension GL_EXT_debug_printf : enable
layout(binding = 2) uniform samplerCube samplerCubeMap;

layout(location = 0) in vec3 inUVW;

layout(location = 0) out vec4 outFragColor;

void main()
{

    outFragColor = pow(texture(samplerCubeMap, inUVW), vec4(1. / 1));
    // debugPrintfEXT("message  %f %f %f  \n", inUVW.x, inUVW.y, inUVW.z);
}