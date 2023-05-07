

// #include "lib/vulkan/Include/glm/glm.hpp"
#include "tt.h"
#include "Data_struct.h"
const int
    tlas = 0,
    out_image = 1;

const int
    eGlobals = 0, // Global uniform containing camera matrices
    eObjDescs = 1, // Access to the object descriptions
    eTextures = 2;

struct V_P_Matrix {
    int s;
};
