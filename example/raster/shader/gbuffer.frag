#version 460
#extension GL_EXT_debug_printf : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_nonuniform_qualifier : enable

#include "Binding.h"
#include "Constants.h"
#include "Shader/Data_struct.h"
#include "Shader/PBR/common.glsl"
layout(location = e_pos) in vec3 in_pos;
layout(location = e_nrm) in vec3 in_nrm;

layout(location = e_texCoord) in vec2 in_texCoord;

layout(location = e_tangentMatrix) in mat3 in_tangentMatrix;

layout(location = e_gbuffer_albedo_renderTarget) out vec4 out_albedo;
layout(location = e_gbuffer_pos_renderTarget) out vec4 out_pos;
layout(location = e_gbuffer_normal_renderTarget) out vec4 out_nrm;
layout(location = e_gbuffer_arm_renderTarget) out vec4 out_arm;

layout(location = 0) out vec4 out_test;

// layout(location = e_gbuffer_tangentMatrix0_renderTarget) out vec4 out_tangentMatrix0;
// layout(location = e_gbuffer_tangentMatrix1_renderTarget) out vec4 out_tangentMatrix1;
// layout(location = e_gbuffer_tangentMatrix2_renderTarget) out vec4 out_tangentMatrix2;
layout(std430, push_constant) uniform _PushContant
{
    PC_Raster pc_raster;
};
layout(set = main_set, binding = e_textures) uniform sampler2D[] textures;

// void encode(inout uvec4 out_tangentMatrix, in mat3 in_tangentMatrix)
// {
//     uint r = floatBitsToUint(in_tangentMatrix[0][0]);
//     r << 8;
//     r += floatBitsToUint(in_tangentMatrix[0][1]);
//     r << 8;
//     r += floatBitsToUint(in_tangentMatrix[0][2]);

//     uint g = floatBitsToUint(in_tangentMatrix[1][0]);
//     g << 8;
//     g += floatBitsToUint(in_tangentMatrix[1][1]);
//     g << 8;
//     g += floatBitsToUint(in_tangentMatrix[1][2]);

//     uint b = floatBitsToUint(in_tangentMatrix[2][0]);
//     b << 8;
//     b += floatBitsToUint(in_tangentMatrix[2][1]);
//     b << 8;
//     b += floatBitsToUint(in_tangentMatrix[2][2]);
//     out_tangentMatrix = uvec4(r, g, b, 1);
// }

void main()
{

    // out_test = vec4(1, 0, 0, 1);
    out_albedo = texture(textures[nonuniformEXT(pc_raster.color_texture_index)], in_texCoord);
    // out_albedo = vec4(1, 0, 0, 1);

    // debugPrintfEXT("message %d \n", pc_raster.metallicness_roughness_texture_index);
    out_arm = texture(textures[nonuniformEXT(pc_raster.metallicness_roughness_texture_index)], in_texCoord);

    out_pos = vec4(in_pos, 1);
    // out_pos = vec4(0, 1, 0, 1);
    vec3 N = texture(textures[nonuniformEXT(pc_raster.normal_texture_index)], in_texCoord).xyz;
    N = normalize(2.0 * N - 1);
    out_nrm = vec4(normalize(in_tangentMatrix * N), 1);
    // debugPrintfEXT("message %f %f %f\n", out_nrm.x, out_nrm.y, out_nrm.z);
    // out_tangentMatrix0 = vec4(in_tangentMatrix[0], 1);
    // out_tangentMatrix1 = vec4(in_tangentMatrix[1], 1);
    // out_tangentMatrix2 = vec4(in_tangentMatrix[2], 1);

    // encode(out_tangentMatrix, in_tangentMatrix);
    // debugPrintfEXT("message \n");
}