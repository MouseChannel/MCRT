
#ifndef DATA_STRUCT
#define DATA_STRUCT
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

// BEGIN_ENUM(Global_Binding)
// e_camera = 0,
//     e_obj_addresses = 1,
//     eTextures = 2 END_ENUM();

BEGIN_ENUM(Gbuffer_Index)
position = 0,
    normal = 1,
    gbuffer_count = 2 END_ENUM();

BEGIN_ENUM(Vertex_Binding)
e_pos = 0,
    e_nrm = 1,
    e_color = 2,
    e_texCoord = 3,
    e_depth = 4 END_ENUM();
struct Camera_data {

    mat4 viewInverse; // Camera inverse view matrix

    vec4 camera_pos;
    vec4 camera_front;
    float fov_angel;
};
struct Camera_matrix {

    mat4 view;
    mat4 project;
    vec3 camera_pos;
};

// struct PushContant {

//     // vec4 clearColor;
//     vec4 lightPosition;
//     float lightIntensity;
//     int frame;
// };
// struct PushContant_Compute {
//     int frame;
//     int open_filter;
// };

// struct Address {
//     // int txtOffset; // Texture index offset in the array of textures
//     uint64_t triangle_count;
//     uint64_t vertexAddress; // Address of the Vertex buffer
//     uint64_t indexAddress; // Address of the index buffer
//     uint64_t materialAddress; // Address of the material buffer
//     uint64_t materialIndexAddress; // Address of the triangle material
// };
struct Vertex // See ObjLoader, copy of VertexObj, could be compressed for device
{
    vec3 pos;
    vec3 nrm;
    vec3 color;
    // int texture_index;
    vec2 texCoord;
#ifdef __cplusplus

    static vk::VertexInputBindingDescription make_bind()
    {
        auto res = vk::VertexInputBindingDescription()
                       .setInputRate(vk::VertexInputRate ::eVertex)
                       .setBinding(0)
                       .setStride(sizeof(Vertex));
        return res;
    }
    static std::vector<vk::VertexInputAttributeDescription> make_attr()
    {
        std::vector<vk::VertexInputAttributeDescription> res_attr(4);
        res_attr[(int)Vertex_Binding::e_pos] = vk::VertexInputAttributeDescription()
                                                   .setBinding(0)
                                                   .setLocation((int)Vertex_Binding::e_pos)
                                                   .setFormat(vk::Format ::eR32G32B32Sfloat)
                                                   .setOffset(offsetof(Vertex, pos));
        res_attr[(int)Vertex_Binding::e_nrm] = vk::VertexInputAttributeDescription()
                                                   .setBinding(0)
                                                   .setLocation((int)Vertex_Binding::e_nrm)
                                                   .setFormat(vk::Format ::eR32G32B32Sfloat)
                                                   .setOffset(offsetof(Vertex, nrm));
        res_attr[(int)Vertex_Binding::e_color] = vk::VertexInputAttributeDescription()
                                                     .setBinding(0)
                                                     .setLocation((int)Vertex_Binding::e_color)
                                                     .setFormat(vk::Format ::eR32G32B32Sfloat)
                                                     .setOffset(offsetof(Vertex, color));
        // res_attr[e_texture_index] = vk::VertexInputAttributeDescription()
        //                                 .setBinding(0)
        //                                 .setLocation(e_texture_index)
        //                                 .setFormat(vk::Format ::eR32Sint)
        //                                 .setOffset(offsetof(Vertex, texture_index));
        res_attr[(int)Vertex_Binding::e_texCoord] = vk::VertexInputAttributeDescription()
                                                        .setBinding(0)
                                                        .setLocation((int)Vertex_Binding::e_texCoord)
                                                        .setFormat(vk::Format ::eR32G32Sfloat)
                                                        .setOffset(offsetof(Vertex, texCoord));
        return res_attr;
    }

#endif
};
struct Material {

    vec4 color;
    vec4 emit;
    bool reflect;
    int color_texture_index;
    int normal_texture_index;
    int metallicness_roughness_texture_index;
};
#endif
