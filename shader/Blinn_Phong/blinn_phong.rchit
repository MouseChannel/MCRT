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
#include "hit_payload.glsl"

hitAttributeEXT vec2 attribs;

layout(location = 0) rayPayloadInEXT hitPayload prd;
layout(location = 1) rayPayloadEXT bool isShadowed;
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
    vec3 color = pow(material.material.color.xyz, vec3(2.2));
    vec3 ambient = 0.05f * color;
    vec3 lightDir = vec3(0);
    vec3 light_atten_coff = vec3(0);
    if (false) {
        // directional light

        lightDir = normalize(pc.pcRay.lightPosition.xyz);
        light_atten_coff = vec3(pc.pcRay.lightIntensity);
    } else {
        // point light
        lightDir = normalize(pc.pcRay.lightPosition.xyz - cur_world_pos);
        light_atten_coff = vec3(pc.pcRay.lightIntensity) / pow(length(pc.pcRay.lightPosition.xyz - cur_world_pos), 2.);
    }

    float diff = max(dot(lightDir, cur_world_normal), 0.);

    vec3 diffuse = diff * light_atten_coff * color;
    vec3 viewDir = normalize(camera_data.camera_pos.xyz - cur_world_pos);
    vec3 halfDir = normalize((lightDir + viewDir));
    float spec = pow(max(dot(halfDir, cur_world_normal), 0.), 32.);
    vec3 specular = 3 * light_atten_coff * spec;

    vec3 radiance = vec3(0);

    if (dot(cur_world_normal, lightDir) > 0) {
        float tMin = 0.001;
        float tMax = length(pc.pcRay.lightPosition.xyz - cur_world_pos);
        vec3 origin = cur_world_pos;
        vec3 rayDir = lightDir;
        uint flags = gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT;
        isShadowed = true;
        traceRayEXT(topLevelAS, // acceleration structure
                    flags, // rayFlags
                    0xFF, // cullMask
                    0, // sbtRecordOffset
                    0, // sbtRecordStride
                    1, // missIndex
                    origin, // ray origin
                    tMin, // ray min range
                    rayDir, // ray direction
                    tMax, // ray max range
                    1 // payload (location = 1)
        );

        if (isShadowed) {
            // debugPrintfEXT("message  in shadow\n");
            radiance = 0.3 * (ambient + diffuse);
        } else {
            // Specular
            // debugPrintfEXT("message  not in shadow\n");

            radiance = (ambient + diffuse + specular);
        }
    }
    // prd.hitValue = pow(radiance, vec3(1. / 2.2));
    prd.hitValue = radiance;
}