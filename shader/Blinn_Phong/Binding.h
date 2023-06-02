#ifdef __cplusplus
#pragma once
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
    e_gbuffer = 2,
    e_normal_gbuffer = 3 END_ENUM();

// BEGIN_ENUM(Global_Binding)
// e_camera = 0,
//     e_obj_addresses = 1,
//     eTextures = 2 END_ENUM();
// BEGIN_ENUM(Gbuffer_Index)
// position = 0,
//     normal = 1,
//     gbuffer_count = 2 END_ENUM();
