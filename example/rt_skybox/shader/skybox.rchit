
#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_debug_printf : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_ARB_shader_clock : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

#include "../Data_struct.h"

#include "../PBR/common.h"
#include "../common.glsl"
#include "../sampling.glsl"
#include "Binding.h"
#include "hit_payload.glsl"

hitAttributeEXT vec2 attribs;

layout(location = 0) rayPayloadInEXT hitPayload prd;
layout(location = 1) rayPayloadEXT bool isShadowed;
// layout(set = e_ray_global, binding = eTextures) uniform sampler2D textures[];
layout(push_constant) uniform PushContant_
{
    PushContant pcRay;
}
pc;
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

    if (prd.depth > 10) {
        return;
    }
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

    const vec3 cur_world_normal = get_cur_world_normal(a.nrm,
                                                       b.nrm,
                                                       c.nrm,
                                                       attribs,
                                                       gl_WorldToObjectEXT);

    mat3 normal_coordinate = getNormalSpace(cur_world_normal);
    uvec3 seed = uvec3(gl_LaunchIDEXT.xy, uint(clockARB()));
    vec3 local_dir = hemisphereSample_cos(seed);
    vec3 rayDirection = normal_coordinate * local_dir;
    rayDirection = reflect(gl_WorldRayDirectionEXT, cur_world_normal);

    vec3 L = rayDirection;
    vec3 V = -gl_WorldRayDirectionEXT;
    vec3 N = cur_world_normal;
    // total specular
    vec3 baseColor = vec3(1, 1, 1);
    float roughness = 0.1;
    float metallicness = 1.0;
    vec3 BRDF = microfacetBRDF(L, V, N, baseColor, metallicness, roughness);

    float cos_theta = dot(rayDirection, cur_world_normal);
    float pdf = cos_theta / PI;

    prd.depth++;
    prd.ray_dir = rayDirection;
    traceRayEXT(topLevelAS, // acceleration structure
                gl_RayFlagsOpaqueEXT, // rayFlags
                0xFF, // cullMask
                0, // sbtRecordOffset
                0, // sbtRecordStride
                0, // missIndex
                cur_world_pos, // ray origin
                1e-3, // ray min range
                rayDirection, // ray direction
                1e5, // ray max range
                0 // payload (location = 0)
    );
    // prd.hitValue = prd.hitValue * BRDF * cos_theta / pdf;
}