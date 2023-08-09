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
#include "hit_payload.glsl"

hitAttributeEXT vec2 attribs;

layout(location = 0) rayPayloadInEXT hitPayload prd;
layout(location = 1) rayPayloadEXT bool isShadowed;
// layout(set = e_ray_global, binding = eTextures) uniform sampler2D textures[];
layout(push_constant) uniform PushContant_
{
    AOPushContant pcRay;
};
layout(set = e_ray_tracing, binding = e_tlas) uniform accelerationStructureEXT topLevelAS;

layout(set = e_ray_global, binding = eTextures) uniform sampler2D textures[];

layout(set = e_ray_global, binding = e_camera) uniform _camera_data
{

    Camera_data camera_data;
};

layout(set = e_ray_global, binding = e_obj_addresses, scalar) buffer _Address
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

    const vec3 cur_world_normal = get_cur_world_normal(a.nrm,
                                                       b.nrm,
                                                       c.nrm,
                                                       attribs,
                                                       gl_WorldToObjectEXT);
    const vec2 cur_uv = get_cur_uv(a.texCoord,
                                   b.texCoord,
                                   c.texCoord,
                                   attribs);
    vec3 color;
    // Lambertian reflection
    if (material.material.texture_index > -1) {

        color = texture(textures[nonuniformEXT(material.material.texture_index)], cur_uv).xyz / PI;

    } else {

        color = material.material.color.xyz / PI;
    }

    mat3 normal_coordinate = getNormalSpace(cur_world_normal);
    uvec3 seed = uvec3(gl_LaunchIDEXT.xy, uint(clockARB()));
    float cc = 1;
    for (int i = 0; i < pcRay.sample_count; i++) {
        seed = uvec3(pcg3d_random(seed) * float(0xffffffffu));
        vec3 local_dir = hemisphereSample_cos(seed);
        vec3 rayDirection = normal_coordinate * local_dir;
        isShadowed = true;
        traceRayEXT(topLevelAS, // acceleration structure
                    gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT, // rayFlags
                    0xFF, // cullMask
                    0, // sbtRecordOffset
                    0, // sbtRecordStride
                    1, // missIndex
                    cur_world_pos, // ray origin
                    0.0001, // ray min range
                    rayDirection, // ray direction
                    pcRay.ao_field, // ray max range
                    1 // payload (location = 1)
        );
        if (isShadowed) {
            cc -= 1. / float(pcRay.sample_count);
        }
    }
    // debugPrintfEXT("message %d \n", pcRay.sample_count);
    prd.hitValue = color;
    prd.cc = cc;
}