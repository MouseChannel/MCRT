#ifndef RT_PBR_PC
#define RT_PBR_PC

#ifdef __cplusplus
#include "glm/glm.hpp"

using vec4 = glm::vec4;
#endif

// struct PushContant_rtpbr {

//     int apply_normal;

//     int use_normal_map;
//     int use_abedo, use_RM_map;

//     vec4 camera_pos;
//     vec4 lightPosition;

//     float roughness;

//     float metallicness;

//     int frame;
// };
struct PushContant_Cloud{
    vec4 lightPosition;
    int frame;
    int offset;
    float noise_scale;
    float  thickness;

};
struct PushContant_Compute {
    int frame;
    int open_filter;
};
#endif
