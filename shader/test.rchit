#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_debug_printf : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

#include "Data_struct.h"
#include "common.glsl"

hitAttributeEXT vec2 attribs;

layout(location = 0) rayPayloadInEXT hitPayload prd;

layout(set = 0, binding = tlas) uniform accelerationStructureEXT topLevelAS;

// layout(set = 1, binding = eObjDescs, scalar) buffer _Address
// {
//     Address address[];
// }
// addresses;

// layout(set = 0, binding = 2) readonly buffer _InstanceInfo
// {
//     // PrimMeshInfo primInfo[];
//     int temp;
// };

// layout(buffer_reference, scalar) readonly buffer _Vertices
// {
//     Vertex vertices[];
// };
// layout(buffer_reference, scalar) buffer _Indices
// {
//     ivec3 indices[];
// };
void main()
{

    // Address address = addresses.address[gl_InstanceCustomIndexEXT];
    // _Vertices vertices = _Vertices(address.vertexAddress);
    // _Indices indices = _Indices(address.indexAddress);

    // ivec3 cur_indices = indices.indices[gl_PrimitiveID];

    // Vertex a = vertices.vertices[cur_indices.x];

    // Vertex b = vertices.vertices[cur_indices.y];

    // Vertex c = vertices.vertices[cur_indices.z];

    // const vec3 cur_world_pos = get_cur_world_position(a.pos,
    //                                                   b.pos,
    //                                                   c.pos,
    //                                                   attribs,
    //                                                   gl_ObjectToWorldEXT);

    // const vec3 cur_world_normal = get_cur_world_normal(a.nrm,
    //                                                    b.nrm,
    //                                                    c.nrm,
    //                                                    attribs,
    //                                                    gl_WorldToObjectEXT);

    // debugPrintfEXT("message  %d %d\n", gl_LaunchIDEXT.x, gl_LaunchIDEXT.y);
    if (gl_LaunchIDEXT.x == 390 && gl_LaunchIDEXT.y == 383) {

        debugPrintfEXT("message from close hitworld_ray %f %f %f \n", gl_WorldRayOriginEXT.x, gl_WorldRayDirectionEXT.y, gl_WorldRayDirectionEXT.z);

        debugPrintfEXT("message from close hitobject_ray %f %f %f \n", gl_ObjectRayOriginEXT.x, gl_ObjectRayOriginEXT.y, gl_ObjectRayOriginEXT.z);
    }
    prd.hitValue = vec3(1, 0, 1);
}