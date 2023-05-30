#version 460

#extension GL_EXT_debug_printf : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#include "../Data_struct.h"
#include "Binding.h"

layout(location = e_nrm) in vec3 in_nrm;

layout(location = e_texCoord) in vec2 in_texCoord;

layout(binding = 0) uniform sampler2D Sampler;
layout(location = 0) out vec4 outColor;
// layout(set = 0, binding = 0, rgba32f) uniform image2D image;
void main()
{

    // vec4 color = imageLoad(image, ivec2(100));
    // if (inUV.x < 1e5 && inUV.y < 1e5)
    //     debugPrintfEXT("message from frag %f %f %f %f\n", color.x, color.y, color.z, color.w);

    outColor = pow(texture(Sampler, in_texCoord).rgba, vec4(1. / 2.2));
}