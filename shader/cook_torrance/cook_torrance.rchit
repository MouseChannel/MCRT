#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_debug_printf : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_ARB_shader_clock : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

#include "../Data_struct.h"
#include "../common.glsl"

#include "../sampling.glsl"
#include "common.glsl"
#include "push_constants/push_constants.h"

#include "hit_payload.glsl"

hitAttributeEXT vec2 attribs;

layout(location = 0) rayPayloadInEXT hitPayload prd;
layout(push_constant) uniform _PushContant
{
    PushContant_cook_torrance pcRay;
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

    const vec3 cur_world_normal = get_cur_world_normal(a.nrm,
                                                       b.nrm,
                                                       c.nrm,
                                                       attribs,
                                                       gl_WorldToObjectEXT);

    {
        if (prd.point_normal.x < 0) {
            prd.point_normal = cur_world_normal;
            prd.point_position = cur_world_pos;
        }
    }

    mat3 normal_coordinate = getNormalSpace(cur_world_normal);
    uvec3 seed = uvec3(gl_LaunchIDEXT.xy, uint(clockARB()));
    vec3 local_dir = hemisphereSample_cos(seed);
    vec3 rayDirection = normal_coordinate * local_dir;
    // Lambertian reflection
    vec3 BRDF = material.material.color.xyz / PI;
    vec3 temp = BRDF;
    if (gl_InstanceCustomIndexEXT == 1) {
        // cook torrance BRDF
        float metallicness = pcRay.metallicness;
        float fresnelReflect = 1.;
        float roughness = pcRay.roughness;
        vec3 color = material.material.color.xyz;
        vec3 lightDir = normalize(pcRay.lightPosition.xyz - cur_world_pos);
        vec3 viewDir = normalize(-gl_WorldRayDirectionEXT);
        BRDF = microfacetBRDF(lightDir, viewDir, cur_world_normal, color, metallicness, fresnelReflect, roughness);
        // debugPrintfEXT("message %f | \n", roughness);
        // debugPrintfEXT("message %f %f %f |%f %f %f | \n", temp.x, temp.y, temp.z, BRDF.x, BRDF.y, BRDF.z);
    }
    float cos_theta = dot(rayDirection, cur_world_normal);
    float pdf = 1. / PI;
    vec3 radiance = BRDF * cos_theta / pdf;
    prd.rayOrigin = cur_world_pos;
    prd.rayDirection = rayDirection;

    if (material.material.emit.x > 1e-6) {

        prd.hitValue = material.material.emit.xyz * prd.weight;

        prd.depth = 100;
    }

    prd.weight *= BRDF * cos_theta / pdf;
    /*
    simple path traing
    shade(p)
        if ray hit the light
            return light_emit * BRDF * cosine / pdf
        else if ray hit an object at q
            return shade(q) * BRDF * cosine / pdf
    */
}