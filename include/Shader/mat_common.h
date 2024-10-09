
#ifdef __cplusplus
#pragma once
#define BEGIN_ENUM(a) enum class a {
#define END_ENUM() }
#include <glm/glm.hpp>
using mat4 = glm::mat4;
using vec4 = glm::vec4;
#else
#define BEGIN_ENUM(a) const uint
#define END_ENUM()
#endif

vec4 to_NDC(vec4 pos)
{
    return pos / pos.w;
}
vec4 to_ScreenPos(vec4 ndc_pos, int screenHeight, int screenWidth)
{
    mat4 screenMat = mat4(1);
    screenMat[0][0] = screenWidth / 2.;
    screenMat[1][1] = screenHeight / 2.;
    screenMat[2][2] = 1. / 2.;
    screenMat[0][3] = screenWidth / 2.;

    screenMat[1][3] = screenHeight / 2.;

    screenMat[2][3] = 1 / 2.;
    screenMat[3][3] = 1.;

    return screenMat * ndc_pos;
}