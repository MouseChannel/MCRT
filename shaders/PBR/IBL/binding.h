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
    e_irradiance_image = 2 END_ENUM();



const int LUT_SIZE = 512;