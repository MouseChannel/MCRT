#version 460

//#extension GL_EXT_debug_printf : enable
#extension GL_GOOGLE_include_directive : enable
//#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_nonuniform_qualifier : enable
#include "Binding.h"
#include "Constants.h"
#include "Data_struct.h"
#include "sampling.glsl"

#include "common.h"
layout(location = e_pos) in vec3 in_pos;
layout(location = e_nrm) in vec3 in_nrm;
layout(location = e_texCoord) in vec2 in_texCoord;
layout(location = e_skybox_uv) in vec3 in_skybox_uv;

//layout(set = e_graphic, binding = e_textures) uniform sampler2D[] textures;
layout(set = e_graphic, binding = e_albedo_image) uniform sampler2D  albedo_image;
layout(set = e_graphic, binding = e_normal_image) uniform sampler2D  normal_image;
layout(set = e_graphic, binding = e_m_r_image) uniform sampler2D  m_r_image;
layout(set = e_graphic, binding = e_skybox) uniform samplerCube skybox;
layout(location = 0) out vec4 outColor;
layout(push_constant) uniform _PushContant
{
    PC_Raster pc_raster;
};
layout(set = e_graphic, binding = e_irradiance_image) uniform samplerCube irradiance_cubemap;
layout(set = e_graphic, binding = e_LUT_image) uniform sampler2D LUT_image;

void main()
{

    vec3 modify_uv = vec3(in_skybox_uv.xy, -in_skybox_uv.z);
    vec4 color = texture(skybox, normalize(modify_uv));
    color = texture(skybox, modify_uv);

    // blinn phong
    // {
    //     vec4 ambient = 0.5f * color;
    //     vec3 light_dir = normalize(pc_raster.light_pos - in_pos);
    //     vec3 view_dir = normalize(pc_raster.camera_pos - in_pos);
    //     vec4 diffuse = max(dot(light_dir, in_nrm), 0.) * color;
    //     vec3 halfDir = normalize((light_dir + view_dir));
    //     float spec = pow(max(dot(halfDir, in_nrm), 0.), 16.);
    //     vec4 specular = spec * vec4(2);
    //     outColor = ambient + diffuse + specular;
    // }

    {
        float roughness = 1;
        float metallicness = 1;
        if (pc_raster.metallicness_roughness_texture_index > -1 && bool(pc_raster.use_r_m_map)) {
            vec2 r_m = texture(m_r_image, in_texCoord).yz;
            roughness = r_m[0];
            metallicness = r_m[1];
            // debugPrintfEXT("message m_r \n");
        }

        vec3 iinormal = in_nrm;
        if (pc_raster.normal_texture_index > -1 && bool(pc_raster.use_normal_map)) {
            iinormal = getNormalSpace(in_nrm) * texture(normal_image, in_texCoord).xyz;
        }
        // }

        vec3 albedo = vec3(1, 1, 1);
        if (pc_raster.color_texture_index > -1) {
            albedo = texture(albedo_image, in_texCoord).xyz;
        }

        vec3 color = Get_IBLColor(vec3(pc_raster.camera_pos),
                                  vec3(0, 0, 10),
                                  in_pos,
                                  iinormal,
                                  metallicness,
                                  roughness,
                                  albedo,
                                  skybox,
                                  irradiance_cubemap,
                                  LUT_image);
        // outColor = 2 * vec4(color, 1);
        if (bool(pc_raster.use_normal_map)) {

            outColor = pow(vec4(color, 1), vec4(1. / 2.2));
        } else {
            outColor = vec4(color, 1);
        }
    }
    // outColor = color;
}