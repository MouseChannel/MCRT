#ifdef __cplusplus

#include "Data_struct.h"
#endif
#ifndef PI
#define PI 3.1415926
#endif
float get_radian(float angel)
{
    return angel / 180. * PI;
}
vec3 get_camera_dir(vec2 launchID, vec2 launchSize, Camera_data camera_data)
{
    vec3 ray_origin = camera_data.camera_pos.xyz;
    // ray_origin = (camera_data.viewInverse * vec4(0, 0, 0, 1)).xyz;
    vec2 pixelCenter = vec2(launchID.xy) + vec2(0.5);
    vec2 inUV = pixelCenter / vec2(launchSize.xy);
    vec2 d = inUV * 2.0 - 1.0;
    float aspectRatio = float(launchSize.x) / float(launchSize.y);
    aspectRatio = 1.;
    d.x = d.x * aspectRatio;
    vec3 ray_dir = vec3(0, 0, -1) * (1. / tan(get_radian(camera_data.fov_angel / 2.))) + vec3(d, 0);
    ray_dir = (camera_data.viewInverse * vec4(ray_dir.xyz, 0)).xyz;
    return ray_dir;
}

vec3 get_barycentrics(vec2 attribs)
{
    return vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);
}
vec3 get_interpolation(vec3 triangle_a, vec3 triangle_b, vec3 triangle_c, vec2 attribs)
{
    vec3 barycentrics = get_barycentrics(attribs);
    return triangle_a * barycentrics.x +
        triangle_b * barycentrics.y +
        triangle_c * barycentrics.z;
}
vec2 get_interpolation(vec2 triangle_a, vec2 triangle_b, vec2 triangle_c, vec2 attribs)
{
    vec3 barycentrics = get_barycentrics(attribs);
    return triangle_a * barycentrics.x +
        triangle_b * barycentrics.y +
        triangle_c * barycentrics.z;
}
vec3 get_cur_world_position(vec3 a_pos, vec3 b_pos, vec3 c_pos, vec2 attribs, mat4x3 object_to_world)
{

    vec3 cur_object_pos = get_interpolation(a_pos, b_pos, c_pos, attribs);
    return object_to_world * vec4(cur_object_pos, 1.0);
}

vec3 get_cur_world_normal(vec3 a_normal, vec3 b_normal, vec3 c_normal, vec2 attribs, mat4x3 world_to_object)
{
    vec3 cur_nrm = get_interpolation(a_normal, b_normal, c_normal, attribs);
    return normalize(vec3(cur_nrm * world_to_object));
}

vec2 get_cur_uv(vec2 a_uv, vec2 b_uv, vec2 c_uv, vec2 attribs)
{
    return get_interpolation(a_uv, b_uv, c_uv, attribs);
}
float get_area(vec3 a_pos, vec3 b_pos, vec3 c_pos)
{

    vec3 e1 = b_pos - a_pos;
    vec3 e2 = c_pos - a_pos;
    return length(cross(e1, e2)) * 0.5f;
    // return object_to_world * vec4(cur_object_pos, 1.0);
}

vec2 directionToSphericalEnvmap(vec3 dir)
{
    float s = fract(1.0 / (2.0 * PI) * atan(dir.y, -dir.x));
    float t = 1.0 / (PI)*acos(-dir.z);
    return vec2(s, t);
}
