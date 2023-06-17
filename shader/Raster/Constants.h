#ifdef __cplusplus

#include "glm/glm.hpp"
using mat4 = glm::mat4;

#endif // DEBUG
struct PC_Raster {
    mat4 model_matrix;
    mat4 view_matrix;
    int texture_index;
};