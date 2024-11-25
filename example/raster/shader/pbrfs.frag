#version 460
#extension GL_EXT_debug_printf : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_nonuniform_qualifier : enable

#include "Binding.h"
#include "Constants.h"
#include "Shader/Data_struct.h"
#include "Shader/PBR/common.glsl"

// layout(location = e_pos) in vec3 in_pos;
// layout(location = e_nrm) in vec3 in_nrm;

// layout(location = e_texCoord) in vec2 in_texCoord;

// layout(location = e_tangentMatrix) in mat3 in_tangentMatrix;

layout(set = main_set, binding = e_textures) uniform sampler2D[] textures;

layout(set = main_set, binding = e_skybox) uniform samplerCube specularTexture;
layout(set = main_set, binding = e_irradiance_image) uniform samplerCube irradianceTexture;
layout(set = main_set, binding = e_LUT_image) uniform sampler2D specularBRDF_LUT;

// layout(set = input_set, input_attachment_index = e_gbuffer_albedo_renderTarget, binding = e_gbuffer_albedo_renderTarget) uniform subpassInput in_albedo;
// layout(set = input_set, input_attachment_index = e_gbuffer_pos_renderTarget, binding = e_gbuffer_pos_renderTarget) uniform subpassInput in_pos;
// layout(set = input_set, input_attachment_index = e_gbuffer_normal_renderTarget, binding = e_gbuffer_normal_renderTarget) uniform subpassInput in_nrm;
// layout(set = input_set, input_attachment_index = e_gbuffer_arm_renderTarget, binding = e_gbuffer_arm_renderTarget) uniform subpassInput in_arm;

layout(set = input_set, input_attachment_index = 0, binding = e_gbuffer_albedo_renderTarget) uniform subpassInput in_albedo;
layout(set = input_set, input_attachment_index = 1, binding = e_gbuffer_pos_renderTarget) uniform subpassInput in_pos;
layout(set = input_set, input_attachment_index = 2, binding = e_gbuffer_normal_renderTarget) uniform subpassInput in_nrm;
layout(set = input_set, input_attachment_index = 3, binding = e_gbuffer_arm_renderTarget) uniform subpassInput in_arm;

// layout(set = input_set, input_attachment_index = e_gbuffer_tangentMatrix0_renderTarget, binding = e_gbuffer_tangentMatrix0_renderTarget) uniform subpassInput in_tangentMatrix0;
// layout(set = input_set, input_attachment_index = e_gbuffer_tangentMatrix1_renderTarget, binding = e_gbuffer_tangentMatrix1_renderTarget) uniform subpassInput in_tangentMatrix1;
// layout(set = input_set, input_attachment_index = e_gbuffer_tangentMatrix2_renderTarget, binding = e_gbuffer_tangentMatrix2_renderTarget) uniform subpassInput in_tangentMatrix2;

layout(std430, push_constant) uniform _PushContant
{
    PC_Raster pc_raster;
};
layout(location = 0) out vec4 color;
layout(set = main_set, binding = e_camera_matrix) uniform _Camera_matrix
{
    Camera_matrix camera_matrix;
};

// void decode(uvec4 out_tangentMatrix, inout mat3 in_tangentMatrix)
// {
//     float target = uintBitsToFloat(out_tangentMatrix[0]);
//         in_tangentMatrix[0][0] = uintBitsToFloat(in_tangentMatrix[0][0]);
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
    //     color = vec4(1, 1, 0, 1);

    vec3 pos = subpassLoad(in_pos).xyz;
    if (pos.x == 0.f) {
        discard;
    }
    
    vec3 N = subpassLoad(in_nrm).xyz;
    // N = normalize(2.0 * N - 1);
    vec3 arm = subpassLoad(in_arm).xyz;
    

    if (bool(pc_raster.use_normal_map)) {
        // N = normalize(tangentMatrix * N);
    }
    vec3 albedo = vec3(1);
    albedo = subpassLoad(in_albedo).rgb;
    if (pc_raster.color_texture_index > -1) {
    }

    vec3 ambientLighting =
        Get_IBLColor(camera_matrix.camera_pos,
                     pos,
                     N,
                     arm.z,
                     arm.y,
                     albedo,
                     specularTexture,
                     irradianceTexture,
                     specularBRDF_LUT);
    color = vec4(ambientLighting, 1);
    if (bool(pc_raster.use_AO)) {
        color = vec4(ambientLighting * arm.x, 1);
    }

     
}
