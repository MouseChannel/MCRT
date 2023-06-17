

#ifdef __cplusplus
#pragma once
#include "glm/glm.hpp"
using vec4 = glm::vec4;
#endif

struct PushContant_light {

    vec4 lightPosition;
    bool has_light;
    float light_desity;

    int frame;
    int enable_indir;
    int enable_dir;
    int enable_filter;
};