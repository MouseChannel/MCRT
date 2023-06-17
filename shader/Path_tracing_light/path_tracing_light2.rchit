#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_debug_printf : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_ARB_shader_clock : enable
#extension GL_EXT_nonuniform_qualifier : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

#include "../Data_struct.h"
// #include "../Set_binding.h"
#include "../common.glsl"
#include "../sampling.glsl"
#include "Binding.h"
#include "Push_constants.h"
#include "hit_payload.glsl"

hitAttributeEXT vec2 attribs;

// layout(location = 0) rayPayloadInEXT hitPayload prd;

layout(location = 1) rayPayloadInEXT hitPayload_light prd_light;
layout(push_constant) uniform _PushContant
{
    PushContant_light pcRay;
};
layout(set = e_ray_tracing, binding = e_tlas) uniform accelerationStructureEXT topLevelAS;
layout(set = e_ray_global, binding = eTextures) uniform sampler2D textures[];

layout(set = e_ray_global, binding = e_obj_addresses, scalar) buffer _Address
{
    Address address[];
}
addresses;
// layout(set = )
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
    if (material.material.emit.x < 1e-3) {
        return;
    }
    float total_area = 0;

    for (int i = 0; i < address.triangle_count; i++) {
        ivec3 cur_indices = indices.indices[i];

        Vertex a = vertices.vertices[cur_indices.x];

        Vertex b = vertices.vertices[cur_indices.y];

        Vertex c = vertices.vertices[cur_indices.z];

        const vec3 cur_world_pos = get_cur_world_position(a.pos,
                                                          b.pos,
                                                          c.pos,
                                                          vec2(0.3, 0.3),
                                                          gl_ObjectToWorldEXT);

        const vec3 cur_world_normal = get_cur_world_normal(a.nrm,
                                                           b.nrm,
                                                           c.nrm,
                                                           vec2(0.3, 0.3),
                                                           gl_WorldToObjectEXT);
        const vec2 cur_uv = get_cur_uv(a.texCoord,
                                       b.texCoord,
                                       c.texCoord,
                                       vec2(0.3, 0.3));

        vec3 incoming_ray_dir = normalize(gl_WorldRayOriginEXT - cur_world_pos);

        if (dot(incoming_ray_dir, cur_world_normal) < 0) {
            continue;
        }

        float pdf = 1. / get_area(a.pos, b.pos, c.pos);
        float cos_theta = dot(prd_light.world_normal, incoming_ray_dir);
        float _cos_theta = dot(cur_world_normal, -incoming_ray_dir);
        float sqr_distance = pow(length(cur_world_pos - gl_WorldRayDirectionEXT), 6);
        prd_light.total_light_contribute += material.material.emit.xyz * (pcRay.light_desity) * cos_theta * _cos_theta / sqr_distance / pdf;
        // debugPrintfEXT("message %f \n", prd_light.total_light_contribute);
    }
}