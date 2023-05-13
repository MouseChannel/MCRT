#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_debug_printf : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

#include "Data_struct.h"
#include "common.glsl"
#include "sampling.glsl"

hitAttributeEXT vec2 attribs;

layout(location = 0) rayPayloadInEXT hitPayload prd;

layout(set = 0, binding = e_tlas) uniform accelerationStructureEXT topLevelAS;

layout(set = 1, binding = e_obj_addresses, scalar) buffer _Address
{
    Address address[];
}
addresses;

// layout(set = 0, binding = 2) readonly buffer _InstanceInfo
// {
//     // PrimMeshInfo primInfo[];
//     int temp;
// };

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

    const vec3 cur_world_normal = get_cur_world_normal(a.nrm,
                                                       b.nrm,
                                                       c.nrm,
                                                       attribs,
                                                       gl_WorldToObjectEXT);

    // Pick a random direction from here and keep going.
    vec3 tangent, bitangent;
    createCoordinateSystem(cur_world_normal, tangent, bitangent);

    vec3 rayDirection = samplingHemisphere(prd.seed, tangent, bitangent, cur_world_normal);
    vec3 BRDF = material.material.color.xyz / M_PI;
    float cos_theta = dot(rayDirection, cur_world_normal);
    prd.rayOrigin = cur_world_pos;
    prd.rayDirection = rayDirection;
    prd.hitValue = material.material.emit.xyz * 3;

    prd.weight = BRDF * cos_theta / (1 / M_PI);
    return;

    // prd.hitValue = vec3(material.material.color);
}