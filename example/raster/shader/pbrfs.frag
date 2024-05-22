#version 460
#extension GL_EXT_debug_printf : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_nonuniform_qualifier : enable

#include "Binding.h"
#include "Constants.h"
#include "shaders/Data_struct.h"
#include "shaders/PBR/common.glsl"
layout(location = e_pos) in vec3 in_pos;
layout(location = e_nrm) in vec3 in_nrm;

layout(location = e_texCoord) in vec2 in_texCoord;

layout(location = e_tangentMatrix) in mat3 in_tangentMatrix;
layout(set = e_graphic, binding = e_textures) uniform sampler2D[] textures;

// layout(set = e_graphic, binding = e_albedo_texture) uniform sampler2D albedo_textures;
// layout(set = e_graphic, binding = e_nrm_texture) uniform sampler2D normal_textures;
// layout(set = e_graphic, binding = e_arm_texture) uniform sampler2D arm_textures;

layout(set = e_graphic, binding = e_skybox) uniform samplerCube specularTexture;
layout(set = e_graphic, binding = e_irradiance_image) uniform samplerCube irradianceTexture;
layout(set = e_graphic, binding = e_LUT_image) uniform sampler2D specularBRDF_LUT;

layout(std430, push_constant) uniform _PushContant
{
    PC_Raster pc_raster;
};
layout(location = 0) out vec4 color;
layout(set = e_graphic, binding = e_camera_matrix) uniform _Camera_matrix
{
    Camera_matrix camera_matrix;
};

void main()
{

    vec3 albedo = vec3(1);
    if (pc_raster.color_texture_index > -1) {

        albedo = texture(textures[nonuniformEXT(pc_raster.color_texture_index)], in_texCoord).xyz;
    }

    // vec3 albedo = texture(albedo_textures, in_texCoord).xyz;

    vec3 r_m = vec3(1);
    if (bool(pc_raster.use_r_m_map)) {
        r_m = texture(textures[nonuniformEXT(pc_raster.metallicness_roughness_texture_index)], in_texCoord).xyz;
    }
    // debugPrintfEXT("%d \n",pc_raster.use_r_m_map);
    // vec3 r_m = texture(arm_textures, in_texCoord).xyz;
    float ao = r_m.x;
    float metalness = r_m.z;
    float roughness = r_m.y;

    vec3 N = in_nrm;
    if (bool(pc_raster.use_normal_map)) {
        N = texture(textures[nonuniformEXT(pc_raster.normal_texture_index)], in_texCoord).xyz;
        N = normalize(2.0 * N - 1);
    }

    N = normalize(in_tangentMatrix * N);

    vec3 ambientLighting =
        Get_IBLColor(camera_matrix.camera_pos,
                     in_pos,
                     N,
                     metalness,
                     roughness,
                     albedo,
                     specularTexture,
                     irradianceTexture,
                     specularBRDF_LUT);

    color = vec4(ambientLighting, 1);
    if (bool(pc_raster.use_AO)) {
        color = vec4(ambientLighting * ao, 1);
    }
}
