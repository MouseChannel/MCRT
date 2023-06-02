#ifdef __cplusplus
#pragma once
#include "glm/glm.hpp"
using vec4 = glm::vec4;
#define BEGIN_ENUM(a) enum a {
#define END_ENUM() }

#else
#define BEGIN_ENUM(a) const uint
#define END_ENUM()
#endif

BEGIN_ENUM(Ray_Tracing_Set)
e_ray_tracing = 0,
    e_ray_global = 1,
    ray_tracing_count = 2 END_ENUM();

// BEGIN_ENUM(Compute_Set)
// e_compute = 0,
//     e_comp_raytracing = 1,
//     compute_count = 2 END_ENUM();
BEGIN_ENUM(Graphic_Set)
e_graphic = 0,
    // e_graphic_global = 1,
    graphic_count = 1 END_ENUM();
BEGIN_ENUM(Ray_Tracing_Binding)
e_tlas = 0,
    e_out_image = 1,
    e_ao_image = 2 END_ENUM();

struct AOPushContant {

    int frame;
    int enable_ao;
    int sample_count;
    float ao_field;
};