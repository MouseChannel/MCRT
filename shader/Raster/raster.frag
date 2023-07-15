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
layout(location = e_out_pos) in vec3 in_pos;
layout(location = e_out_uv) in vec3 in_uv;
layout(set = e_graphic, binding = e_textures) uniform sampler2D[] textures;
layout(binding = 3) uniform samplerCube samplerCubeMap;
layout(location = 0) out vec4 outColor;
layout(push_constant) uniform _PushContant
{
    PC_Raster pc_raster;
};

void main()
{

    vec3 modify_uv = vec3(in_uv.xy * 1.0, -in_uv.z);
    vec4 color = texture(samplerCubeMap, normalize(modify_uv));
    // color = vec4(1, 1, 0, 1);
    // blinn phong
    {
        vec4 ambient = 0.5f * color;
        vec3 light_dir = normalize(pc_raster.light_pos - in_pos);
        vec3 view_dir = normalize(pc_raster.camera_pos - in_pos);
        vec4 diffuse = max(dot(light_dir, in_nrm), 0.) * color;
        vec3 halfDir = normalize((light_dir + view_dir));
        float spec = pow(max(dot(halfDir, in_nrm), 0.), 16.);
        vec4 specular = spec * vec4(2);
        outColor = ambient + diffuse + specular;
    }
    outColor = color;
    return;
    if (pc_raster.texture_index >= 0) {
        outColor = pow(texture(textures[nonuniformEXT(pc_raster.texture_index)], in_texCoord).rgba, vec4(1. / 2.2));
        // debugPrintfEXT("message %f\n", in_texCoord.y);
    } else {
        outColor = vec4(1, 1, 1, 1);
    }
}