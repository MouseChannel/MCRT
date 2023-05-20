#ifdef __cplusplus
#include "glm/glm.hpp"
using vec4 = glm::vec4;
#endif

struct PushContant_cook_torrance {

    // vec4 clearColor;
    vec4 lightPosition;
    float lightIntensity;
    int frame;
    float roughness;
    float metallicness;
};