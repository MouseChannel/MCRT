
#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_buffer_reference2 : require

#include "../Data_struct.h"
#include "../common.glsl"
hitAttributeEXT vec2 attribs;

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
layout(location = 0) rayPayloadInEXT hitPayload prd;
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
    vec3 light_pos = vec3(0, 0, 10);
    vec3 light_dir = normalize(light_pos - cur_world_pos);
    prd.miss = false;

    traceRayEXT(topLevelAS, // acceleration structure
                gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsSkipClosestHitShaderEXT, // rayFlags
                0xFF, // cullMask
                0, // sbtRecordOffset
                0, // sbtRecordStride
                0, // missIndex
                cur_world_pos, // ray origin
                0.001, // ray min range
                light_dir, // ray direction
                10000, // ray max range
                0 // payload (location = 0)
    );

    if (prd.miss) {
        prd.hitValue = material.material.color.xyz;
    } else {
        prd.hitValue = vec3(0, 0, 0);
    }
}