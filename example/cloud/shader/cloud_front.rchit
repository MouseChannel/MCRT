#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_debug_printf : enable
#include "Binding.h"
#include "hit_payload.glsl"
#include "shaders/Data_struct.h"
#include "shaders/common.glsl"

hitAttributeEXT vec2 attribs;
layout(location = 0) rayPayloadInEXT hitPayload prd;
layout(set = e_ray_tracing, binding = e_tlas) uniform accelerationStructureEXT topLevelAS;
// layout(set = 0, binding = e_noise_data, rgba32f) uniform image3D noise;
layout(set = 0, binding = e_noise_data) uniform sampler3D noise;

layout(set = 1, binding = e_obj_addresses, scalar) buffer _Address
{
    Address address[];
}
addresses;
layout(buffer_reference, scalar) readonly buffer _Indices
{
    ivec3 indices[];
};
layout(buffer_reference, scalar) readonly buffer _Vertices
{

    Vertex vertices[];
};
void main()
{
    // debugPrintfEXT("message front\n" );
    Address address = addresses.address[gl_InstanceCustomIndexEXT];
    _Vertices vertices = _Vertices(address.vertexAddress);
    _Indices indices = _Indices(address.indexAddress);
    ivec3 cur_indices = indices.indices[gl_PrimitiveID];

    Vertex a = vertices.vertices[cur_indices.x];

    Vertex b = vertices.vertices[cur_indices.y];

    Vertex c = vertices.vertices[cur_indices.z];

    const vec3 cur_world_pos = get_cur_world_position(a.pos,
                                                      b.pos,
                                                      c.pos,
                                                      attribs,
                                                      gl_ObjectToWorldEXT);

    uint rayFlags = gl_RayFlagsNoneEXT;

    prd.ray_origin = cur_world_pos;
    // +  prd.ray_dir*0.1f;

    ivec3 cur_uvw = ivec3(cur_world_pos * 128);
    prd.hitValue = texture(noise, cur_world_pos).xyz;
    // prd.hitValue = imageLoad(noise, cur_uvw).xyz;

    // if (gl_LaunchIDEXT.x == gl_LaunchSizeEXT.x / 2 && gl_LaunchIDEXT.y == gl_LaunchSizeEXT.y / 2) {
    //     prd.hashit = 1;
    //     // debugPrintfEXT("message \n" );
    // }

    // traceRayEXT(topLevelAS, // acceleration structure
    //             rayFlags, // rayFlags
    //             0xFF, // cullMask
    //             1, // sbtRecordOffset '1 for cloud behind here'
    //             0, // sbtRecordStride
    //             1, // missIndex '1 for raymarching here'
    //             prd.ray_origin, // ray origin
    //             0.001f, // ray min range
    //             prd.ray_dir, // ray direction
    //             0.1f, // ray max range
    //             0 // payload (location = 0)
    // );
}