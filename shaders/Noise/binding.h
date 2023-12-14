#ifdef __cplusplus

#define BEGIN_ENUM(a) enum class a {
#define END_ENUM() }

#else
#define BEGIN_ENUM(a) const uint
#define END_ENUM()
#endif

BEGIN_ENUM(NoiseBinding)
e_permutations = 0,
e_noiseData = 1 END_ENUM();


struct PushContant_Noise {
    int doing_filter;
};