#ifdef __cplusplus
#include "glm/glm.hpp"

using vec4 = glm::vec4;
#endif

struct PushContant_rtpbr {

    // vec4 clearColor;
    vec4 camera_pos;
    vec4 lightPosition;
    float roughness;
    float metallicness;
    int frame;
    
};