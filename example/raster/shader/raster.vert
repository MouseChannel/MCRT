#version 460
#extension GL_EXT_debug_printf : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_GOOGLE_include_directive : enable

#include "Binding.h"
#include "shaders/Data_struct.h"

#include "shaders/AntiAliasing/TAA/Halton_2_3.h"
#include "Constants.h"
#include "shaders/mat_common.h"
layout(location = e_pos) in vec3 in_pos;
layout(location = e_nrm) in vec3 in_nrm;
layout(location = e_color) in vec3 in_color;
layout(location = e_texCoord) in vec2 in_texCoord;

layout(location = e_nrm) out vec3 out_nrm;
layout(location = e_pos) out vec3 out_pos;
layout(location = e_depth) out float out_depth;
layout(location = e_skybox_uv) out vec3 out_skybox_uv;

layout(location = e_texCoord) out vec2 out_texCoord;

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
    mat4 model_matrix = pc_raster.model_matrix;
    mat4 view_matrix = pc_raster.view_matrix;
    mat4 project_matrix = camera_matrix.project;

    gl_Position = project_matrix * view_matrix * model_matrix * vec4(in_pos, 1.);

    // vec4 before = project_matrix * view_matrix * model_matrix * vec4(in_pos, 1.);
    // mat4 jmat= jitterMat(1,1200,800,project_matrix);
    // vec4 after  = jmat *view_matrix * model_matrix * vec4(in_pos, 1.);
    
    // vec4 ndc_pos = to_NDC(before);
    // vec4 screen_pos = to_ScreenPos(ndc_pos,800,1200);
//     debugPrintfEXT("message %f %f %f %f | %f %f %f %f| %f %f %f %f\n",before.x,before.y,before.z,before.w, ndc_pos.x,ndc_pos.y,ndc_pos.z,ndc_pos.w,screen_pos.x,screen_pos.y,screen_pos.z,screen_pos.w);
    // mat4 ee = mat4(1 );
    // debugPrintfEXT("message %f %f %f %f | %f %f %f %f |%f %f %f %f | %f %f %f %f\n",ee[0][0],ee[0][1],ee[0][2],ee[0][3],ee[1][0],ee[1][1],ee[1][2],ee[1][3],ee[2][0],ee[2][1],ee[2][2],ee[2][3],ee[3][0],ee[3][1],ee[3][2],ee[3][3]);


    // vec3 carmera_dir = pc_raster.camera_pos - in_pos;
    vec4 world_pos = model_matrix * vec4(in_pos, 1.);
    vec4 world_nrm = model_matrix * vec4(in_nrm, 1.);
    vec3 carmera_dir = vec3
    (pc_raster.camera_pos) - vec3(world_pos);

    vec3 new_uv = vec3(model_matrix * vec4(reflect(carmera_dir, vec3(world_nrm)), 1));
    // vec3 new_uv = vec3(model_matrix * vec4(reflect(carmera_dir, vec3(model_matrix * vec4(in_nrm, 1))), 1));

    // out_uv = new_uv;

    out_pos = vec3(world_pos);
    out_nrm = vec3(world_nrm);

    out_texCoord = in_texCoord;
    out_skybox_uv = new_uv;

     vec4 ndc_pos = to_NDC(gl_Position);
    out_depth = ndc_pos.z;
}