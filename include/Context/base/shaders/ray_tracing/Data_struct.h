
#ifndef RT_DATA_STRUCT
#define RT_DATA_STRUCT
#ifdef __cplusplus
#pragma once
#include "glm/glm.hpp"
#include "vulkan/vulkan.hpp"
using mat4 = glm::mat4;
using vec4 = glm::vec4;

using vec3 = glm::vec3;
using vec2 = glm::vec2;
using ivec3 = glm::ivec3;
// using uint63 = unsigned long;
using mat4x3 = glm::mat4x3;
using uint = unsigned int;
#endif

#ifdef __cplusplus

#define BEGIN_ENUM(a) enum class a {
#define END_ENUM() }

#else
#define BEGIN_ENUM(a) const uint
#define END_ENUM()
#endif

BEGIN_ENUM(Global_Binding)
e_camera = 0,
    e_obj_addresses = 1,
    eTextures = 2 END_ENUM();

// struct Camera_data {

//     mat4 viewInverse; // Camera inverse view matrix
//     vec4 camera_pos;
//     vec4 camera_front;
//     float fov_angel;
// };

struct Address {
    // int txtOffset; // Texture index offset in the array of textures
    uint64_t triangle_count;
    uint64_t vertexAddress; // Address of the Vertex buffer
    uint64_t indexAddress; // Address of the index buffer
    uint64_t materialAddress; // Address of the material buffer
    uint64_t materialIndexAddress; // Address of the triangle material
};

struct PushContant {

    // vec4 clearColor;
    vec4 lightPosition;
    float lightIntensity;
    int frame;
};

#endif
