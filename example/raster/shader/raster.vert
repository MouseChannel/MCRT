#version 460 core
#extension GL_EXT_debug_printf : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_GOOGLE_include_directive : enable

#include "Binding.h"
#include "shaders/Data_struct.h"

#include "Constants.h"
// #include "shaders/AntiAliasing/TAA/Halton_2_3.h"
#include "shaders/mat_common.h"
layout(location = e_pos) in vec3 in_pos;
layout(location = e_nrm) in vec3 in_nrm;
layout(location = e_tangent) in vec3 in_tangent;
layout(location = e_bitangent) in vec3 in_bitangent;
layout(location = e_color) in vec3 in_color;
layout(location = e_texCoord) in vec2 in_texCoord;

layout(location = e_pos) out vec3 out_pos;
layout(location = e_nrm) out vec3 out_nrm;
layout(location = e_tangent) out vec3 out_tangent;
layout(location = e_bitangent) out vec3 out_bitangnet;

// layout(location = e_depth) out float out_depth;
layout(location = e_texCoord) out vec2 out_texCoord;
layout(location = e_tangentMatrix) out mat3 out_tangentMatrix;
// layout(location = e_skybox_uv) out vec3 out_skybox_uv;

layout(push_constant) uniform _PushContant
{
    PC_Raster pc_raster;
};
layout(set = e_graphic, binding = e_camera_matrix) uniform _Camera_matrix
{
    Camera_matrix camera_matrix;
};
// layout(location = e_out_uv) out vec3 out_uv;
// layout(location = e_out_pos) out vec3 out_pos;

void main()
{
    // debugPrintfEXT("message1 \n");
    mat4 model_matrix = pc_raster.model_matrix;
    mat4 view_matrix = camera_matrix.view;
    mat4 project_matrix = camera_matrix.project;

    gl_Position = project_matrix * view_matrix * model_matrix * vec4(in_pos, 1.);
    // gl_Position =   model_matrix * vec4(in_pos, 1.);

    // debugPrintfEXT("%f %f %f |%f %f %f |%f %f %f |\n", view_matrix[0][0], view_matrix[0][1], view_matrix[0][2],

    //                view_matrix[1][0],
    //                view_matrix[1][1],
    //                view_matrix[1][2],
    //                view_matrix[2][0],
    //                view_matrix[2][1],
    //                view_matrix[2][2]);

    debugPrintfEXT("%f %f %f\n", gl_Position.x, gl_Position.y, gl_Position.z);

    vec4 world_pos = model_matrix * vec4(in_pos, 1.);
    vec4 world_nrm = model_matrix * vec4(in_nrm, 1.);
    // {//skybox uv
    //     vec3 carmera_dir = vec3(camera_matrix.camera_pos) - vec3(world_pos);
    //     vec3 new_uv = vec3(model_matrix * vec4(reflect(carmera_dir, vec3(world_nrm)), 1));
    //     out_skybox_uv = new_uv;
    // }
    out_pos = vec3(world_pos);
    out_nrm = vec3(world_nrm);
    out_texCoord = in_texCoord;
    out_tangentMatrix = mat3(model_matrix) * mat3(in_tangent, in_bitangent, in_nrm);
    { // for gbuffer depth
        vec4 ndc_pos = to_NDC(gl_Position);
        // out_depth = ndc_pos.z;
    }
}