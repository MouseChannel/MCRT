#version 460
#extension GL_EXT_debug_printf : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_GOOGLE_include_directive : enable

#include "../Data_struct.h"
#include "Binding.h"
layout(location = e_pos) in vec3 in_pos;
layout(location = e_nrm) in vec3 in_nrm;
layout(location = e_color) in vec3 in_color;
layout(location = e_texCoord) in vec2 in_texCoord;

layout(location = e_nrm) out vec3 out_nrm;
layout(location = e_texCoord) out vec2 out_texCoord;
layout(push_constant) uniform _PushContant
{
    PC_Raster pc_raster;
};
layout(set = e_graphic, binding = e_camera_matrix) uniform _Camera_matrix
{
    Camera_matrix camera_matrix;
};

void main()
{
    mat4 model_matrix = pc_raster.model_matrix;
    mat4 view_matrix = camera_matrix.view;
    mat4 project_matrix = camera_matrix.project;

    gl_Position = project_matrix * view_matrix * model_matrix * vec4(in_pos, 1.);
    out_nrm = in_nrm;
    out_texCoord = in_texCoord;
}