
#ifndef DATA_STRUCT
#define DATA_STRUCT
#ifdef __cplusplus
#include "glm/glm.hpp"

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

#define BEGIN_ENUM(a) enum a {
#define END_ENUM() }

#else
#define BEGIN_ENUM(a) const uint
#define END_ENUM()
#endif

BEGIN_ENUM(Ray_Tracing_Binding)
e_tlas = 0,
    e_out_image = 1 END_ENUM();

BEGIN_ENUM(Global_Binding)
e_camera = 0,
    e_obj_addresses = 1,
    eTextures = 2 END_ENUM();

struct hitPayload {
    vec3 hitValue;

    vec3 rayOrigin;
    uint seed;
    uint depth;
    vec3 rayDirection;
    vec3 weight;
};
struct Camera_data {
    // int s;
    // int rr;
    mat4 viewInverse; // Camera inverse view matrix
    mat4 projInverse;
    vec4 camera_pos;
    vec4 camera_front;
};

struct PushContant_Ray {
    int frame;
    vec4 clearColor;
    vec4 lightPosition;
    float lightIntensity;
    int lightType;
};

struct Address {
    // int txtOffset; // Texture index offset in the array of textures
    uint64_t test_address;
    uint64_t vertexAddress; // Address of the Vertex buffer
    uint64_t indexAddress; // Address of the index buffer
    uint64_t materialAddress; // Address of the material buffer
    uint64_t materialIndexAddress; // Address of the triangle material
};
struct Vertex // See ObjLoader, copy of VertexObj, could be compressed for device
{
    vec3 pos;
    vec3 nrm;
    vec3 color;
    vec2 texCoord;
};
struct Material {

    vec4 color;
    vec4 emit;
};
#endif
