

#ifdef __cplusplus
#pragma once
#define BEGIN_ENUM(a) enum a {
#define END_ENUM() }

#else
#define BEGIN_ENUM(a) const uint
#define END_ENUM()
#endif
BEGIN_ENUM(Compute_Binding)
e_tlas = 0,
    e_out_image = 1,
    e_ao_image = 2,
    END_ENUM();


BEGIN_ENUM(Compute_Set)
e_compute = 0,
    e_comp_raytracing = 1,
    compute_count = 2 END_ENUM();