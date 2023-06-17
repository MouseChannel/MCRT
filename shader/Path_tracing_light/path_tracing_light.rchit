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

#include "../common.glsl"
#include "../sampling.glsl"
#include "Binding.h"
#include "Push_constants.h"
#include "hit_payload.glsl"

hitAttributeEXT vec2 attribs;

layout(location = 0) rayPayloadInEXT hitPayload prd;
// layout(location = 1) rayPayloadEXT hitPayload_light prd_light;
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
    // debugPrintfEXT("message %d %d \n", pcRay.enable_dir, pcRay.enable_indir);
    if (prd.depth > 4) {
        // debugPrintfEXT("message12 \n");
        return;
    }
    Address address = addresses.address[gl_InstanceCustomIndexEXT];
    _Vertices vertices = _Vertices(address.vertexAddress);
    _Indices indices = _Indices(address.indexAddress);
    _Material material = _Material(address.materialAddress);

    if (material.material.emit.x > 1e-5) {
        prd.hitValue = vec3(0);

        return;
    }
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
    const vec2 cur_uv = get_cur_uv(a.texCoord,
                                   b.texCoord,
                                   c.texCoord,
                                   attribs);

    // fill gbuffer
    {
        if (prd.point_normal.x < 0) {
            prd.point_normal = cur_world_normal;
            prd.point_position = cur_world_pos;
            // prd.hitValue = cur_world_pos;
            // prd.depth = 100;
            // return;
        }
    }
    vec3 dir = vec3(0);
    vec3 indir = vec3(0);
    vec3 color = vec3(0, 0, 0);
    if (material.material.texture_index > -1) {

        color = texture(textures[nonuniformEXT(material.material.texture_index)], cur_uv).xyz / PI;

    } else {

        color = material.material.color.xyz / PI;
    }
    if (pcRay.enable_dir > 0) {
        // debugPrintfEXT("message enable_dir \n");
        // light Sample
        prd_light.world_normal = cur_world_normal;
        prd_light.total_light_contribute = vec3(0);
        float light_distance = length(pcRay.lightPosition.xyz - cur_world_pos);
        vec3 light_dir = pcRay.lightPosition.xyz - cur_world_pos;

        traceRayEXT(topLevelAS, // acceleration structure
                    gl_RayFlagsNoneEXT, // rayFlags
                    0xFF, // cullMask
                    1, // sbtRecordOffset
                    0, // sbtRecordStride
                    1, // missIndex
                    cur_world_pos, // ray origin
                    1e-3, // ray min range
                    light_dir, // ray direction
                    light_distance, // ray max range
                    1 // payload
        );
        dir = prd_light.total_light_contribute * color;
        prd.dir = dir;
    }
    if (pcRay.enable_indir > 0) {
        // hemisphere Sample
        // debugPrintfEXT("message enable_indir \n");

        mat3 normal_coordinate = getNormalSpace(cur_world_normal);
        uvec3 seed = uvec3(gl_LaunchIDEXT.xy, uint(clockARB()));
        vec3 local_dir = hemisphereSample_cos(seed);
        vec3 rayDirection = normal_coordinate * local_dir;

        float cos_theta = dot(rayDirection, cur_world_normal);

        float pdf = cos_theta / PI;
        prd.rayOrigin = cur_world_pos;
        prd.rayDirection = rayDirection;

        prd.depth++;

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

        // indir = prd.hitValue * color * cos_theta / pdf;
        vec3 incoming = prd.hitValue;
        prd.indir = incoming * color * cos_theta / pdf;
    }

    prd.hitValue = prd.dir + prd.indir;
    // prd.hitValue = dir + indir;
}