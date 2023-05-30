#ifdef __cplusplus

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

BEGIN_ENUM(Compute_Set)
e_compute = 0,
    e_comp_raytracing = 1,
    compute_count = 2 END_ENUM();
BEGIN_ENUM(Graphic_Set)
e_graphic = 0,
    // e_graphic_global = 1,
    graphic_count = 1 END_ENUM();
