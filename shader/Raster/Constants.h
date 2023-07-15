#ifdef __cplusplus

#include "glm/glm.hpp"
using mat4 = glm::mat4;
using vec3 = glm::vec3;
#endif // DEBUG
struct PC_Raster {
    mat4 model_matrix;

    mat4 model_inverse_matrix;
    mat4 view_matrix;
    vec3 camera_pos;

    int texture_index;
    vec3 light_pos;
};