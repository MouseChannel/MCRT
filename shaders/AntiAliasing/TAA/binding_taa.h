
#ifdef __cplusplus
#pragma once
#define BEGIN_ENUM(a) enum class a {
#define END_ENUM() }
#include <glm/glm.hpp>
using mat4 = glm::mat4;
#else
#define BEGIN_ENUM(a) const uint
#define END_ENUM()
#endif

BEGIN_ENUM(TAA_Binding)
e_offscreen_image = 0,
    e_pre_image = 1,
    e_out_image = 2,
    e_taa_data = 3,
    // e_gbuffer = 3,
    // gbuffer
    e_last_depth = 4,
    e_gbuffer_position = 5 END_ENUM();

// BEGIN_ENUM(Gbuffer_Binding)
// gbuffer_position = 0,
//     gbuffer_normal = 1,
//     gbuffer_last_depth = 2,
//     gbuffer_color = 3,
//     gbuffer_count = 4 END_ENUM();

struct TAA_Data {
    mat4 last_mv;
    mat4 last_p;
    mat4 cur_mv;
    mat4 cur_p;
    int offset_index;
    int screenHeight;
    int screenWidth;
};