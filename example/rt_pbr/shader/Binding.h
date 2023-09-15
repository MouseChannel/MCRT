#ifdef __cplusplus

#define BEGIN_ENUM(a) enum class a {
#define END_ENUM() }

#else
#define BEGIN_ENUM(a) const uint
#define END_ENUM()
#endif

#include "example/base/shader/ray_tracing/Data_struct.h"

BEGIN_ENUM(Ray_Tracing_Set)
e_ray_tracing = 0,
    e_ray_global = 1,
    ray_tracing_count = 2 END_ENUM();
BEGIN_ENUM(Ray_Tracing_Binding)
e_tlas = 0,
    e_out_image = 1,
    e_rt_skybox = 2,
    e_rt_LUT_image = 3,
    e_rt_irradiance_image = 4 END_ENUM();