#version 460

#extension GL_EXT_debug_printf : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#include "shaders/Data_struct.h"
#extension GL_EXT_nonuniform_qualifier : enable
#include "Binding.h"
#include "Constants.h"
#include "shaders/sampling.glsl"
// #include "Binding.h"
#include "shaders/PBR/common.h"
layout(location = e_nrm) in vec3 in_nrm;
// layout(location = e_texture_index) in flat int in_texture_index;
layout(location = e_texCoord) in vec2 in_texCoord;
layout(location = e_out_pos) in vec3 in_pos;
layout(location = e_out_uv) in vec3 in_uv;
layout(set = e_graphic, binding = e_textures) uniform sampler2D[] textures;
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

    vec3 modify_uv = vec3(in_uv.xy, -in_uv.z);
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
        if (pc_raster.metallicness_roughness_texture_index > -1) {
            vec2 r_m = texture(textures[nonuniformEXT(pc_raster.metallicness_roughness_texture_index)], in_texCoord).yz;
            roughness = r_m[0];
            metallicness = r_m[1];
        }
        // debugPrintfEXT("message %d \n", pcRay.use_normal_map);
        // if (bool(pcRay.use_normal_map)) {
        // debugPrintfEXT("message \n");
        vec3 iinormal;
        if (pc_raster.normal_texture_index > -1) {
            iinormal = getNormalSpace(in_nrm) * texture(textures[nonuniformEXT(pc_raster.normal_texture_index)], in_texCoord).xyz;
        }
        // }

        vec3 albedo = vec3(1, 1, 1);
        if (pc_raster.color_texture_index > -1) {
            albedo = texture(textures[nonuniformEXT(pc_raster.color_texture_index)], in_texCoord).xyz / PI;
        }
        vec3 N = iinormal;
        vec3 V = normalize(vec3(pc_raster.camera_pos) - in_pos);
        vec3 L = normalize(vec3(pc_raster.light_pos) - in_pos);
        vec3 H = normalize(V + L);

        vec3 F0 = vec3(0.04);
        F0 = mix(F0, albedo, metallicness);
        vec3 F = fresnelSchlick(max(dot(V, H), 0.0), F0);
        vec3 KS = F;
        vec3 KD = vec3(1.0) - KS;

        vec3 irradiance = texture(irradiance_cubemap, N).rgb;
        vec3 diffuse = albedo * irradiance;
        // vec3 ray_dir = reflect(gl_WorldRayDirectionEXT, in_nrm);

        // vec3 prefilteredColor = textureLod(skybox, ray_dir, roughness * 11).rgb;
        vec3 prefilteredColor = textureLod(skybox, modify_uv, roughness * 11).rgb;
        vec3 brdf = texture(LUT_image, vec2(max(dot(N, V), 0.0), roughness)).rgb;
        vec3 specular = prefilteredColor * (F0 * brdf.r + brdf.g);
        outColor = vec4(KD * diffuse + specular, 1);
    }
    // outColor = color;
}