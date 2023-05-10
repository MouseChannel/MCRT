// #ifdef __cplusplus

// #include "Data_struct.h"
// #endif

struct hitPayload {
    vec3 hitValue;
};

vec3 get_camera_ray(float fov, float aspect, vec2 point)
{
    float focalLength = 1.0 / tan(0.5 * fov * 3.14159265359 / 180.0);
    // compute position in the camera's image plane in range [-1.0, 1.0]
    vec2 pos = 2.0 * (point)-1;
    return normalize(vec3(pos.x * aspect, pos.y, -focalLength));

    // return vec3(1);
}
