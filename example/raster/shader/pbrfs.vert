#version 450 core
// Physically Based Rendering
// Copyright (c) 2017-2018 Michał Siejak

// Physically Based shading model: Vertex program.
#extension GL_EXT_scalar_block_layout : require
#include "Binding.h"
#include "shaders/Data_struct.h"

#include "Constants.h"
// #include "shaders/AntiAliasing/TAA/Halton_2_3.h"
#include "shaders/mat_common.h"
layout(location = e_pos) in vec3 in_pos;
layout(location = e_nrm) in vec3 in_nrm;
layout(location = e_tangent) in vec3 in_tangent;
layout(location = e_bitangent) in vec3 in_bitangent;
// layout(location = e_color) in vec3 in_color;
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
layout(set = e_graphic, std430, binding = e_camera_matrix) uniform _Camera_matrix
{
    Camera_matrix camera_matrix;
};

void main()
{

    out_pos = vec3(in_pos);
    out_nrm = vec3(in_nrm);
    out_texCoord = vec2(in_texCoord.x, 1 - in_texCoord.y);

    out_tangentMatrix = mat3(in_tangent, in_bitangent, in_nrm);
    gl_Position = camera_matrix.project * camera_matrix.view * vec4(in_pos, 1.);
   
}
