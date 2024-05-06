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
layout(location = e_depth) in float in_depth;
layout(location = e_texCoord) in vec2 in_texCoord;
layout(location = e_skybox_uv) in vec3 in_skybox_uv;

layout(set = e_graphic, binding = e_textures) uniform sampler2D[] textures;
layout(set = e_graphic, binding = e_skybox) uniform samplerCube skybox;
layout(location = 0) out vec4 outColor;
layout(location  = 1)out vec4 gbuffer_color;

layout(location  = 2)out vec4 gbuffer_position;
layout(location  = 3)out vec4 gbuffer_depth;

layout(set = e_graphic, binding = e_camera_matrix) uniform _Camera_matrix
{
    Camera_matrix camera_matrix;
};

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
        float metallicness = 1;
        float ambient_occlusion= 1;
        if (pc_raster.metallicness_roughness_texture_index > -1 && bool(pc_raster.use_r_m_map)) {
            vec3 r_m = texture(textures[nonuniformEXT(pc_raster.metallicness_roughness_texture_index)], in_texCoord).xyz;
            
            ambient_occlusion = r_m.x;
            // roughness = r_m.y;
            // metallicness = r_m.z;
            roughness  =r_m.y;
            metallicness = r_m.z;
            // metallicness = 0.25;
            // roughness = 0.25;
            // roughness = 0;
            // metallicness = 1;

            // metallicness = 0;
            // debugPrintfEXT("message  %f %f\n",metallicness, roughness );
        }

        vec3 iinormal =  normalize( in_nrm);
        if (pc_raster.normal_texture_index > -1 && bool(pc_raster.use_normal_map)) {
            iinormal = getNormalSpace(in_nrm) * texture(textures[nonuniformEXT(pc_raster.normal_texture_index)], in_texCoord).xyz;
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

        // if (bool(pc_raster.use_AO)){

        //     color *=ambient_occlusion;
        // }

        outColor = pow(vec4(color, alpha), vec4(1. / pc_raster.gamma));
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