#version 460

#extension GL_EXT_debug_printf : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_nonuniform_qualifier : enable
#include "Binding.h"
#include "Constants.h"
#include "shaders/Data_struct.h"
#include "shaders/sampling.glsl"

#include "shaders/PBR/common.h"

layout(location = e_pos) in vec3 in_pos;
layout(location = e_nrm) in vec3 in_nrm;
// layout(location = e_depth) in float in_depth;
layout(location = e_texCoord) in vec2 in_texCoord1;
// layout(location = e_skybox_uv) in vec3 in_skybox_uv;
layout(location = e_tangentMatrix) in mat3 in_tangentMatrix;

layout(set = e_graphic, binding = e_textures) uniform sampler2D[] textures;
layout(set = e_graphic, binding = e_skybox) uniform samplerCube skybox;
layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 gbuffer_color;

layout(location = 2) out vec4 gbuffer_position;
layout(location = 3) out vec4 gbuffer_depth;

layout(set = e_graphic, binding = e_camera_matrix) uniform _Camera_matrix
{
    Camera_matrix camera_matrix;
};

layout(std430, push_constant) uniform _PushContant
{
    PC_Raster pc_raster;
};
layout(set = e_graphic, binding = e_irradiance_image) uniform samplerCube irradiance_cubemap;
layout(set = e_graphic, binding = e_LUT_image) uniform sampler2D LUT_image;

void main()
{
    vec2 in_texCoord = vec2(0.3,0.7);
    in_texCoord = in_texCoord1;

    vec3 sam = texture(skybox, vec3(0.3, 0.5, 0.6)).rgb;
    // debugPrintfEXT("%f %f %f\n", sam.x, sam.y, sam.z);
    // debugPrintfEXT("message \n");
    // vec3 modify_uv = vec3(in_skybox_uv.xy, -in_skybox_uv.z);
    // vec4 color = texture(skybox, normalize(modify_uv));
    // color = texture(skybox, modify_uv);
    //    debugPrintfEXT("messagein test");
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
        float metallicness = 1.;
        float ambient_occlusion = 1;
        if (pc_raster.metallicness_roughness_texture_index > -1 && bool(pc_raster.use_r_m_map)) {
            vec3 r_m = texture(textures[nonuniformEXT(pc_raster.metallicness_roughness_texture_index)], in_texCoord).xyz;

            ambient_occlusion = r_m.x;
            roughness = r_m.y;
            metallicness = r_m.z;
            // vec3 temp = texture(textures[nonuniformEXT(pc_raster.metallicness_roughness_texture_index)], vec2(0.2, 0.3)).xyz;

            // debugPrintfEXT("message %f %f %f\n", temp.x, temp.y, temp.z);
            // roughness = 0;
            // metallicness = 0;
            // debugPrintfEXT("%f %f \n", roughness,metallicness);

            // if (gl_FragCoord.x > 599 && gl_FragCoord.x < 601 && gl_FragCoord.y > 399 && gl_FragCoord.y < 401)
            //     debugPrintfEXT("message metallicness %f %f\n", metallicness, roughness);
            // debugPrintfEXT("%f %f \n", gl_FragCoord.x, gl_FragCoord.y);
            // if(metallicness>0.8){
            //    debugPrintfEXT("%f %f \n", gl_FragCoord.x, gl_FragCoord.y);
            // }
        }

        vec3 iinormal = normalize(in_nrm);
        if (pc_raster.normal_texture_index > -1 && bool(pc_raster.use_normal_map)) {
            iinormal = in_tangentMatrix * (texture(textures[nonuniformEXT(pc_raster.normal_texture_index)], in_texCoord).xyz);
            iinormal = normalize(iinormal);

            // debugPrintfEXT("messagelength%f \n",length(normalize( in_nrm)) );
        }
        // }
        vec3 albedo = vec3(1, 1, 1);
        float alpha = 1.f;
        if (pc_raster.color_texture_index > -1) {
            vec4 color = texture(textures[nonuniformEXT(pc_raster.color_texture_index)], in_texCoord);
            albedo = color.xyz;
            alpha = color.w;
        }
        // debugPrintfEXT("message  f %f %f\n", camera_matrix.camera_pos.x, camera_matrix.camera_pos.y, camera_matrix.camera_pos.z);

        vec3 color = Get_IBLColor(
            vec3(camera_matrix.camera_pos),
            in_pos,
            iinormal,
            metallicness,
            roughness,
            albedo,
            skybox,
            irradiance_cubemap,
            LUT_image);
        ambient_occlusion = 1;
        if (bool(pc_raster.use_AO)) {
            // debugPrintfEXT("messag/home/mousechannel/project/MCRT/assets/chair.glbe ao %f\n", ambient_occlusion);
            color *= ambient_occlusion;
            float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
            float mappedLuminance = (luminance * (1.0 + luminance / (1.f * 1.f))) / (1.0 + luminance);

            // Scale color by ratio of average luminances.
            vec3 mappedColor = (mappedLuminance / luminance) * color;
            outColor = vec4(mappedColor, 1);
            return;
        }
        outColor = vec4(pow(color, vec3(1. / pc_raster.gamma)), 1);
        outColor = vec4(pow(color, vec3(1. / 2.2)), 1);

        // outColor = vec4(iinormal,1 );
    }
    // debugPrintfEXT("message %f %f %f \n",color.x,color.y,color.z );
    // gbuffer_color = outColor;
    // gbuffer_position = vec4(in_pos, 1);
    // gbuffer_depth = vec4(in_depth);

    //    if(bool(pc_raster.use_r_m_map)){
    //    gbuffer_color = vec4(1,0,0,1);
    //    gbuffer_color2 = vec4(0,1,0,1);
    //
    //    }else{
    //     gbuffer_color = vec4(0,1,0,1);
    //         gbuffer_color2 = vec4(0,0,1,1);
    //
    //    }
    // outColor = color;
}