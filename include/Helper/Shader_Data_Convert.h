#pragma once
#include "glm/glm.hpp"
#include "shader/Data_struct.h"

static Mat4f convert_to_shader_data(glm::mat4 data)
{

    return Mat4f {
        data[0][0],
        data[0][1],
        data[0][2],
        data[0][3],
        data[1][0],
        data[1][1],
        data[1][2],
        data[1][3],
        data[2][0],
        data[2][1],
        data[2][2],
        data[2][3],
        data[3][0],
        data[3][1],
        data[3][2],
        data[3][3]
    };
}

template <typename T>
// requires requires(T a) { typename T::shader_data; }
struct Host_Data {
public:
    using shader_data = T;
    virtual T to_shader_data() = 0;
};

struct V_P_Matrix : public Host_Data<VP_Matrix> {
    // using shader_data = Shader_V_P_Matrix;

public:
    V_P_Matrix(glm::mat4 view, glm::mat4 project)
        : viewProj(project)
        , viewInverse(glm::inverse(view))
        , projInverse(glm::inverse(project))

    {
    }
    V_P_Matrix() = default;
    glm::mat4 viewProj; // Camera view * projection
    glm::mat4 viewInverse; // Camera inverse view matrix
    glm::mat4 projInverse;
    shader_data to_shader_data() override
    {
        return shader_data {
            .viewProj {
                convert_to_shader_data(viewProj) },
            .viewInverse {
                convert_to_shader_data(viewInverse) },
            .projInverse {
                convert_to_shader_data(projInverse) },
        };
    }
};
