#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_debug_printf : enable
#include "Binding.h"
#include "Push_Constants.h"
#include "hit_payload.glsl"
#include "shaders/Data_struct.h"
#include "shaders/common.glsl"

hitAttributeEXT vec2 attribs;
layout(location = 0) rayPayloadInEXT hitPayload prd;
layout(location = 1) rayPayloadEXT CloudPayload cloud_payload;
layout(set = e_ray_tracing, binding = e_tlas) uniform accelerationStructureEXT topLevelAS;
layout(set = 0, binding = e_noise_data, rgba32f) uniform image3D noise;
// layout(set = 0, binding = e_noise_data) uniform sampler3D noise;
layout(set = 0, binding = e_rt_skybox) uniform samplerCube samplerCubeMap;
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
layout(push_constant) uniform _PushContant
{
    PushContant_Cloud pcRay;
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

    ivec3 cur_uvw = ivec3(cur_world_pos * 128) % 128;
    // prd.hitValue = texture(noise, cur_world_pos).xyz;
    prd.hitValue = imageLoad(noise, cur_uvw).xyz;

    // if (gl_LaunchIDEXT.x == gl_LaunchSizeEXT.x / 2 && gl_LaunchIDEXT.y == gl_LaunchSizeEXT.y / 2) {
    //     prd.hashit = 1;
    //     // debugPrintfEXT("message \n" );
    // }

    { // for raymarching cloud density
        cloud_payload.depth = 0;
        cloud_payload.ray_origin = cur_world_pos;
        cloud_payload.ray_dir = prd.ray_dir;
        cloud_payload.total_density = 0.;
        cloud_payload.color = vec3(1.f);
        traceRayEXT(topLevelAS, // acceleration structure
                    rayFlags, // rayFlags
                    0xFF, // cullMask
                    1, // sbtRecordOffset '1 for cloud behind here'
                    0, // sbtRecordStride
                    1, // missIndex '1 for cloud raymarching here'
                    cloud_payload.ray_origin, // ray origin
                    0.001f, // ray min range
                    cloud_payload.ray_dir, // ray direction
                    0.1f, // ray max range
                    1 // payload (location = 0)
        );
        if (cloud_payload.color.x < 0.9f)
            debugPrintfEXT("message %f  %f %f\n", cloud_payload.color.x, cloud_payload.color.y, cloud_payload.color.z);
    }

    prd.hitValue = vec3(cloud_payload.total_density) * cloud_payload.color;

    prd.hitValue = texture(samplerCubeMap, normalize(cloud_payload.ray_dir)).rgb * 0.5f;
    cloud_payload.total_density *= pcRay.thickness;

    prd.hitValue = vec3(cloud_payload.total_density) + texture(samplerCubeMap, normalize(cloud_payload.ray_dir)).rgb * vec3(1.f - cloud_payload.total_density);
}