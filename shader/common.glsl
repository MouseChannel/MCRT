#ifdef __cplusplus

#include "Data_struct.h"
#endif

// vec3 get_camera_ray(float fov, float aspect, vec2 point)
// {
//     float focalLength = 1.0 / tan(0.5 * fov * 3.14159265359 / 180.0);
//     // compute position in the camera's image plane in range [-1.0, 1.0]
//     vec2 pos = 2.0 * (point)-1;
//     return normalize(vec3(pos.x * aspect, pos.y, -focalLength));

//     // return vec3(1);
// }
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
uint tea(uint val0, uint val1)
{
    uint v0 = val0;
    uint v1 = val1;
    uint s0 = 0;

    for (uint n = 0; n < 16; n++) {
        s0 += 0x9e3779b9;
        v0 += ((v1 << 4) + 0xa341316c) ^ (v1 + s0) ^ ((v1 >> 5) + 0xc8013ea4);
        v1 += ((v0 << 4) + 0xad90777d) ^ (v0 + s0) ^ ((v0 >> 5) + 0x7e95761e);
    }

    return v0;
}
