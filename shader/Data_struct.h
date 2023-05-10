
#ifndef DATA_STRUCT
#define DATA_STRUCT
#ifdef __cplusplus
#include "glm/glm.hpp"

using mat4 = glm::mat4;
using vec4 = glm::vec4;

using vec3 = glm::vec3;
using vec2 = glm::vec2;

#endif

struct Camera_data {
    // int s;
    // int rr;
    mat4 viewInverse; // Camera inverse view matrix
    mat4 projInverse;
    vec3 camera_pos;
    vec3 camera_front;
};

struct PushContant_Ray {
    vec4 clearColor;
    vec3 lightPosition;
    float lightIntensity;
    int lightType;
};
#endif