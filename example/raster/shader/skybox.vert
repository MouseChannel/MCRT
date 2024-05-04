
#version 460
// #extension GL_EXT_debug_printf : enable
// #extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_GOOGLE_include_directive : enable

#include "Binding.h"
#include "Constants.h"
#include "shaders/Data_struct.h"

layout(location = 0) in vec3 inPos;
//not used
layout(location = e_nrm) in vec3 in_nrm;
layout(location = e_color) in vec3 in_color;
layout(location = e_texCoord) in vec2 in_texCoord;


//layout(push_constant) uniform _PushContant
//{
//    PC_Raster pc_raster;
//};
layout(set = e_graphic, binding = e_camera_matrix) uniform _Camera_matrix
{
    Camera_matrix camera_matrix;
};

layout(location = 0) out vec3 outUVW;

void main()
{
    outUVW = inPos;
    mat4 model_matrix = mat4(1);
    mat4 view_matrix = camera_matrix.view;
    view_matrix[3] = vec4(0, 0, 0, 1);
    mat4 project_matrix = camera_matrix.project;

    gl_Position = project_matrix * view_matrix * model_matrix * vec4(inPos, 1.);
    outUVW.xy *= -1.0;
}
