#version 460

#extension GL_EXT_debug_printf : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#include "../Data_struct.h"
#extension GL_EXT_nonuniform_qualifier : enable
#include "Binding.h"
#include "Constants.h"

layout(location = e_nrm) in vec3 in_nrm;
// layout(location = e_texture_index) in flat int in_texture_index;
layout(location = e_texCoord) in vec2 in_texCoord;

layout(set = e_graphic, binding = e_textures) uniform sampler2D textures[];
layout(location = 0) out vec4 outColor;
layout(push_constant) uniform _PushContant
{
    PC_Raster pc_raster;
};

void main()
{
    // TODO PBR
    // TODO Something weird about texture buffer!!!!
    float c = pc_raster.texture_index;
    outColor = vec4(c / 6., 0, 0, 1);
    return;
    if (pc_raster.texture_index >= 0)

        outColor = pow(texture(textures[0], vec2(in_texCoord.x, abs(in_texCoord.y))).rgba, vec4(1. / 2.2));
    else {
        outColor = vec4(1, 1, 1, 1);
    }
}