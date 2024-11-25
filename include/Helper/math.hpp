#pragma once
// #include "Helper/Shader_Data.h"
#include "Shader/Data_struct.h"
#include "glm/glm.hpp"
#include <cstddef>

static constexpr int align_up(int x, size_t a)
{
    return int((x + (int(a) - 1)) & ~int(a - 1));
}