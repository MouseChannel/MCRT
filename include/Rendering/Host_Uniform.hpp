#include "glm/glm.hpp"
struct V_P_Matrix {
    V_P_Matrix(glm::mat4 view, glm::mat4 project)
        : viewProj(project * view)
        , viewInverse(glm::inverse(view))
        , projInverse(glm::inverse(project))

    {
    }
    glm::mat4 viewProj; // Camera view * projection
    glm::mat4 viewInverse; // Camera inverse view matrix
    glm::mat4 projInverse;
};
