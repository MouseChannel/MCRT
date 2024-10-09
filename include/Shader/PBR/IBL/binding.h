#ifdef __cplusplus

#define BEGIN_ENUM(a) enum a {
#define END_ENUM() }

#else
#define BEGIN_ENUM(a) const uint
#define END_ENUM()
#endif

BEGIN_ENUM(IBL_Binding)
e_skybox = 0,
    e_LUT_image = 1,
    e_irradiance_image = 2,
    e_hdr_image = 3,
    e_skybox_mipmap = 4

    END_ENUM();

BEGIN_ENUM(IBL_SPEC_Binding)
e_numsample_count = 0,
e_local_size = 1

    END_ENUM();

const int local_size = 32;