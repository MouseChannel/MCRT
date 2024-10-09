

#ifdef __cplusplus
#pragma once
#define BEGIN_ENUM(a) enum class a {
#define END_ENUM() }
#include <glm/glm.hpp>
using mat4 = glm::mat4;
using vec2 = glm::vec2;
#else
#define BEGIN_ENUM(a) const uint
#define END_ENUM()
#endif
const vec2 Halton_2_3[8] = {
    vec2(0.0f, -1.0f / 3.0f),
    vec2(-1.0f / 2.0f, 1.0f / 3.0f),
    vec2(1.0f / 2.0f, -7.0f / 9.0f),
    vec2(-3.0f / 4.0f, -1.0f / 9.0f),
    vec2(1.0f / 4.0f, 5.0f / 9.0f),
    vec2(-1.0f / 4.0f, -5.0f / 9.0f),
    vec2(3.0f / 4.0f, 1.0f / 9.0f),
    vec2(-7.0f / 8.0f, 7.0f / 9.0f)
};

mat4 jitterMat(int offset_index, int screenWidth, int screenHeight, mat4 projection)
{
    float deltaWidth = 1.f / screenWidth, deltaHeight = 1.f / screenHeight;
    vec2 jitter = vec2(
        Halton_2_3[offset_index].x * deltaWidth,
        Halton_2_3[offset_index].y * deltaHeight);
    mat4 jitterMat = projection;
    jitterMat[2][0] += jitter.x;
    jitterMat[2][1] += jitter.y;
    // jitterMat[0][2] += jitter.x;
    // jitterMat[1][2] += jitter.y;
    return jitterMat;
}