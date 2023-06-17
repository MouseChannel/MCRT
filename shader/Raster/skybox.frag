
#version 460
#extension GL_EXT_debug_printf : enable
layout(binding = 1) uniform samplerCube samplerCubeMap;

layout(location = 0) in vec3 inUVW;

layout(location = 0) out vec4 outFragColor;

void main()
{
    outFragColor = texture(samplerCubeMap, inUVW);
    debugPrintfEXT("message  %f %f %f  \n", inUVW.x, inUVW.y, inUVW.z);
}