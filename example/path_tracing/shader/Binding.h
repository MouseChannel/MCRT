#ifdef __cplusplus

#define BEGIN_ENUM(a) enum class a {
#define END_ENUM() }

#else
#define BEGIN_ENUM(a) const uint
#define END_ENUM()
#endif

#include "example/base/shaders/ray_tracing/Data_struct.h"
#include "example/path_tracing/shader/Push_constants.h"
BEGIN_ENUM(Ray_Tracing_Set)
e_ray_tracing = 0,
    e_ray_global = 1,
    ray_tracing_count = 2 END_ENUM();

BEGIN_ENUM(Compute_Set)
e_compute = 0,
    e_comp_raytracing = 1,
    compute_count = 2 END_ENUM();

BEGIN_ENUM(Graphic_Set)
e_graphic = 0,
    // e_graphic_global = 1,
    graphic_count = 1 END_ENUM();
BEGIN_ENUM(Binding)
e_tlas = 0,
    e_rt_out_image = 1,
    e_gbuffer = 2,

    e_compute_out_image = 3 END_ENUM();

// BEGIN_ENUM(Gbuffer_Index)
// position = 0,
//     normal = 1,
//     gbuffer_count = 2 END_ENUM();

// struct PushContant_Compute {
//     int frame;
//     int open_filter;
// };
 