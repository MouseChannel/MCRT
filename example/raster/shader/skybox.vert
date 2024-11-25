
#version 450
// #extension GL_EXT_debug_printf : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
// #extension GL_GOOGLE_include_directive : enable

#include "Binding.h"
#include "Constants.h"
#include "Shader/Data_struct.h"

layout(location = e_pos) in vec3 inPos;
layout(location = e_nrm) in vec3 in_nrm;
layout(location = e_texCoord) in vec2 in_texCoord;

layout(set = main_set, binding = e_camera_matrix) uniform _Camera_matrix
{
    Camera_matrix camera_matrix;
};

layout(location = 0) out vec3 outUVW;

void main()
{
    outUVW = inPos;

    mat4 view_matrix = camera_matrix.view;
    view_matrix[3] = vec4(0, 0, 0, 1);

    gl_Position = camera_matrix.project * view_matrix * vec4(inPos, 1.);
    // gl_Position = vec4(1, 1, 1, 1);
}
