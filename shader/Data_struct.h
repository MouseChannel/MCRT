#pragma once

struct Mat4f {
    float data[4][4];
};

struct VP_Matrix {
    Mat4f viewProj; // Camera view * projection
    Mat4f viewInverse; // Camera inverse view matrix
    Mat4f projInverse;
};