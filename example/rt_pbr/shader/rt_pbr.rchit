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

    Address address = addresses.address[gl_InstanceCustomIndexEXT];
    _Vertices vertices = _Vertices(address.vertexAddress);
    _Indices indices = _Indices(address.indexAddress);
    _Material material = _Material(address.materialAddress);

    ivec3 cur_indices = indices.indices[gl_PrimitiveID];

    Vertex a = vertices.vertices[cur_indices.x];

    Vertex b = vertices.vertices[cur_indices.y];

    Vertex c = vertices.vertices[cur_indices.z];

    const vec3 cur_world_pos = get_cur_world_position(a.pos,
                                                      b.pos,
                                                      c.pos,
                                                      attribs,
                                                      gl_ObjectToWorldEXT);

    vec3 cur_world_normal = get_cur_world_normal(a.nrm,
                                                 b.nrm,
                                                 c.nrm,
                                                 attribs,
                                                 gl_WorldToObjectEXT);
    const vec2 cur_uv = get_cur_uv(a.texCoord,
                                   b.texCoord,
                                   c.texCoord,
                                   attribs);

    float roughness = pcRay.roughness;
    float metallicness = pcRay.metallicness;
 
    

    if (bool(pcRay.use_RM_map) && material.material.metallicness_roughness_texture_index > -1) {
        vec2 r_m = texture(textures[nonuniformEXT(material.material.metallicness_roughness_texture_index)], cur_uv).yz;
        roughness = r_m[0];
        metallicness = r_m[1];
       //  debugPrintfEXT("message in rm \n");
    }
   
    // if (bool(pcRay.use_normal_map)) {
        // debugPrintfEXT("message \n");
    if (bool(pcRay.use_normal_map) && material.material.normal_texture_index > -1) {
            cur_world_normal = getNormalSpace(cur_world_normal) * texture(textures[nonuniformEXT(material.material.normal_texture_index)], cur_uv).xyz;
               //    debugPrintfEXT("message in normal \n");
    }
    // }

    vec3 albedo = vec3(1, 1, 1);
    if (bool(pcRay.use_abedo) && material.material.color_texture_index > -1) {
        albedo = texture(textures[nonuniformEXT(material.material.color_texture_index)], cur_uv).xyz / PI;
             //  debugPrintfEXT("message in albedo \n");
    }
    vec3 N = cur_world_normal;
    vec3 V = normalize(vec3(camera_data.camera_pos) - cur_world_pos);
    vec3 L = normalize(vec3(pcRay.lightPosition) - cur_world_pos);
    vec3 H = normalize(V + L);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallicness);
    vec3 F = fresnelSchlick(max(dot(V, H), 0.0), F0);
    vec3 KS = F;
    vec3 KD = vec3(1.0) - KS;

    vec3 irradiance = texture(irradiance_cubemap, N).rgb;
    vec3 diffuse = albedo * irradiance;
    vec3 ray_dir = reflect(gl_WorldRayDirectionEXT, cur_world_normal);

    vec3 prefilteredColor = textureLod(skybox, ray_dir, roughness * 11).rgb;
    vec3 brdf = texture(LUT_image, vec2(max(dot(N, V), 0.0), roughness)).rgb;
    vec3 specular = prefilteredColor * (F0 * brdf.r + brdf.g);

    prd.hitValue = Get_IBLColor(vec3(camera_data.camera_pos),
                                vec3(pcRay.lightPosition),
                                cur_world_pos,
                                cur_world_normal,
                                metallicness,
                                roughness,
                                albedo,
                                skybox,
                                irradiance_cubemap,
                                LUT_image);
                                
    prd.hitValue = pow(prd.hitValue, vec3(1. / 2.2));
    // prd.hitValue = KD * diffuse + specular;
    // prd.hitValue = irradiance;
}