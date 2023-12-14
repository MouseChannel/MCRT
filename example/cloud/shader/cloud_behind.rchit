#version 460

#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_debug_printf : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_ARB_shader_clock : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_nonuniform_qualifier : enable

#include "shaders/Data_struct.h"

#include "Binding.h"
#include "Push_Constants.h"
#include "hit_payload.glsl"
#include "shaders/PBR/common.h"
#include "shaders/common.glsl"

#include "shaders/sampling.glsl"

hitAttributeEXT vec2 attribs;

layout(location = 0) rayPayloadInEXT hitPayload prd;
layout(location = 1) rayPayloadEXT bool isShadowed;
layout(set = e_ray_global, binding = eTextures) uniform sampler2D textures[];
layout(set = e_ray_tracing, binding = e_rt_skybox) uniform samplerCube skybox;
layout(set = e_ray_tracing, binding = e_rt_irradiance_image) uniform samplerCube irradiance_cubemap;
layout(set = e_ray_tracing, binding = e_rt_LUT_image) uniform sampler2D LUT_image;

layout(push_constant) uniform PushContant_
{
    PushContant_rtpbr pcRay;
};

layout(set = 1, binding = e_camera) uniform _camera_data
{
    Camera_data camera_data;
};
layout(set = 0, binding = e_tlas) uniform accelerationStructureEXT topLevelAS;

layout(set = 1, binding = e_obj_addresses, scalar) buffer _Address
{
    Address address[];
}
addresses;

layout(buffer_reference, scalar) readonly buffer _Vertices
{
    Vertex vertices[];
};
layout(buffer_reference, scalar) readonly buffer _Indices
{
    ivec3 indices[];
};
layout(buffer_reference, scalar) readonly buffer _Material
{
    Material material;
};

void main()
{
     
    if( prd.depth >1)
       debugPrintfEXT("message 2 \n");
    // debugPrintfEXT("message behind \n");
    return;

    
}