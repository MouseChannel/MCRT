#ifdef __cplusplus

#include "glm/glm.hpp"
using mat4 = glm::mat4;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
#endif // DEBUG
#ifndef raster_push_constants
#define raster_push_constants
struct PC_Raster {
    mat4 model_matrix;

    mat4 view_matrix;
    vec4 camera_pos;

    int color_texture_index;
    int metallicness_roughness_texture_index;
    int normal_texture_index;
    int rr;
    vec4 light_pos;

    int use_normal_map;
    int use_r_m_map;
    int uu;
    int ee;
};
struct PushContant_Compute {
    int frame;
    int open_filter;
};
#endif