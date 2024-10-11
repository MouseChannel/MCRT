#version 460
#extension GL_EXT_debug_printf : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_nonuniform_qualifier : enable

#include "Binding.h"
#include "Constants.h"
#include "include/Shader/Data_struct.h"
#include "include/Shader/PBR/common.glsl"
layout(location = e_pos) in vec3 in_pos;
layout(location = e_nrm) in vec3 in_nrm;

layout(location = e_texCoord) in vec2 in_texCoord;

layout(location = e_tangentMatrix) in mat3 in_tangentMatrix;


layout(location = 0) out vec4 out_albedo;
layout(location = 1) out vec4 out_pos;
layout(location = 2) out vec4 out_nrm;
layout(std430, push_constant) uniform _PushContant
{
    PC_Raster pc_raster;
};
layout(set = e_graphic, binding = e_textures) uniform sampler2D[] textures;


void main(){
    out_albedo = texture(textures[nonuniformEXT(pc_raster.color_texture_index)], in_texCoord);
    out_pos = vec4(in_pos, 1);


    vec3 N= texture(textures[nonuniformEXT(pc_raster.normal_texture_index)], in_texCoord).xyz;
    N = normalize(2.0 * N - 1);
    out_nrm =vec4(normalize(in_tangentMatrix * N), 1.f);

}