
#version 460
#extension GL_EXT_debug_printf : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_GOOGLE_include_directive : enable

#include "../Data_struct.h"
#include "Binding.h"
#include "Constants.h"
layout(location = 0) in vec3 inPos;

layout(push_constant) uniform _PushContant
{
    PC_Raster pc_raster;
};
layout(set = e_graphic, binding = e_camera_matrix) uniform _Camera_matrix
{
    Camera_matrix camera_matrix;
};

layout(location = 0) out vec3 outUVW;

void main()
{
    outUVW = inPos;
    mat4 model_matrix = pc_raster.model_matrix;
    mat4 view_matrix = pc_raster.view_matrix;
    // mat4 view_matrix = camera_matrix.view;
    mat4 project_matrix = camera_matrix.project;

    outUVW.xy *= -1.0;
    gl_Position = project_matrix * model_matrix * vec4(inPos, 1.);
    // gl_Position = ubo.projection * ubo.model * vec4(inPos.xyz, 1.0);
}
