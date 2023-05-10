

// #include "lib/vulkan/Include/glm/glm.hpp"
#include "tt.h"
// #include "Data_struct.h"

#define print_mat4(mat) \
    debugPrintfEXT("mat4 = [0][0] = %f [0][1] = %f [0][2] = %f [0][3] = %f \n [1][0] = %f [1][1] = %f [1][2] = %f [1][3] = %f \n [2][0] = %f [2][1] = %f [2][2] = %f [2][3] = %f \n [3][0] = %f [3][1] = %f [3][2] = %f [3][3] = %f \n ", mat[0][0], mat[0][1], mat[0][2], mat[0][3] mat[1][0], mat[1][1], mat[1][2], mat[1][3] mat[2][0], mat[2][1], mat[2][2], mat[2][3] mat[3][0], mat[3][1], mat[3][2], mat[3][3]);

const int tlas = 0,
          out_image = 1;

const int
    eGlobals = 0, // Global uniform containing camera matrices
    eObjDescs = 1, // Access to the object descriptions
    eTextures = 2;

// struct V_P_Matrix {
//     int s;
// };
