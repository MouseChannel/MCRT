#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_debug_printf : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_ARB_shader_clock : enable
#extension GL_EXT_nonuniform_qualifier : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

#include "shader/Data_struct.h"

// #include "shader/Set_binding.h"
#include "Binding.h"
#include "hit_payload.glsl"
#include "shader/common.glsl"
#include "shader/sampling.glsl"


hitAttributeEXT vec2 attribs;

layout(location = 0) rayPayloadInEXT hitPayload prd;
// layout(push_constant) uniform _PushContant
// {
//     PushContant pcRay;
// };
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
    // prd.depth++;
    // return;
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
    // prd.hitValue = material.material.color.xyz;
    // prd.depth = 100;
    // return;

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

    mat3 normal_coordinate = getNormalSpace(cur_world_normal);
    uvec3 seed = uvec3(gl_LaunchIDEXT.xy, uint(clockARB()));
    vec3 local_dir = hemisphereSample_cos(seed);
    vec3 rayDirection = normal_coordinate * local_dir;
    vec3 BRDF;
    // Lambertian reflection
    if (material.material.color_texture_index > -1) {

        BRDF = texture(textures[nonuniformEXT(material.material.color_texture_index)], cur_uv).xyz / PI;
        // debugPrintfEXT("message \n");
    } else {

        BRDF = material.material.color.xyz / PI;
    }
    float cos_theta = dot(rayDirection, cur_world_normal);

    float pdf = cos_theta / PI;
    vec3 radiance = BRDF * cos_theta / pdf;
    prd.rayOrigin = cur_world_pos;
    prd.rayDirection = rayDirection;
    prd.depth++;
    if (material.material.emit.x > 1e-6) {

        float d = pow(length(cur_world_pos - gl_WorldRayOriginEXT), 2.);

        prd.hitValue = material.material.emit.xyz * prd.weight;

        prd.depth = 100;
    }

    prd.weight *= radiance;
    /*
    simple path traing
    shade(p)
        if ray hit the light
            return light_emit * BRDF * cosine / pdf
        else if ray hit an object at q
            return shade(q) * BRDF * cosine / pdf
    */
}