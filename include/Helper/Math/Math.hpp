#pragma once

// #include "SDL.h"

#include "Helper/Math/Matrix.hpp"
#include "Helper/Math/Vec.hpp"

#include <stdint.h>
#include <variant>
template <typename T>
concept LerpObject = requires(T a, T b, float weight) {
                         a + b;

                         a* weight;
                     };
class math {
public:
    template <typename T>
    static void swap(T& a, T& b)
    {
        T c;
        c = b;
        b = a;
        a = c;
    }

    template <LerpObject T>
    static T lerp(T x1, T x2, float weight)
    {
        return x1 * (1.0f - weight) + x2 * weight;
    }

    template <LerpObject T>
    static T lerp(T a, T b, T c, float weight_a, float weight_b, float weight_c)
    {
        // assert()
        return a * weight_a + b * weight_b + c * weight_c;
    }

    template <LerpObject T>
    static T lerp(std::array<T, 3> value, std::array<float, 3> weights)
    {
        return lerp(value[0], value[1], value[2], weights[0], weights[1], weights[2]);
    }
    template <LerpObject T>
    static T lerp(std::vector<T> value, std::vector<float> weights)
    {
        assert(value.size() == 3 && weights.size() == 3);
        return lerp(value[0], value[1], value[2], weights[0], weights[1], weights[2]);
    }
    template <LerpObject T>
    static T lerp(T a, T b, T c, std::array<float, 3> weights)
    {

        return lerp(a, b, c, weights[0], weights[1], weights[2]);
    }

    static mat4f
    translate_matrix(Vector3<float> v)
    {

        mat4f translate_matrix { 1.0f };
        translate_matrix.set_column(3, { v, 1.0f });

        return translate_matrix;
    }
    static mat4f scale_matrix(Vector3<float> v)
    {
        mat4f scale_matrix { 1.0f };
        for (int i = 0; i < 3; i++) {
            scale_matrix.set(i, i, v[i]);
        }
        return scale_matrix;
    }
    static mat4f rotate_matrix(float angle, Vector3<float> dir)
    {
        auto axis = dir.normalize();
        auto x = axis[0], y = axis[1], z = axis[2];
        mat4f res = Mat<float>::Identity(4);
        auto sin = std::sin(angle);
        auto cos = std::cos(angle);
        auto one_minux_cos = 1.0f - cos;
        res.set(0, 0, x * x * one_minux_cos + cos);
        res.set(0, 1, x * y * one_minux_cos - z * sin);
        res.set(0, 2, x * z * one_minux_cos + y * sin);
        res.set(1, 0, x * y * one_minux_cos + z * sin);
        res.set(1, 1, y * y * one_minux_cos + cos);
        res.set(1, 2, y * z * one_minux_cos - x * sin);
        res.set(2, 0, x * z * one_minux_cos - y * sin);
        res.set(2, 1, y * z * one_minux_cos + x * sin);
        res.set(2, 2, z * z * one_minux_cos + cos);
        return res;
    }

    /**
     * @brief camera default position at (0,0,0) and look at (0,0,-1)
     *
     * f == -z
     * r == x
     * u == y
     */
    static mat4f view_mat(Vector3<float> position,
                          Vector3<float> front)
    {

        Vector3<float> top = { 0, 1, 0 };
        auto t_m = translate_matrix(position * (-1.0f));
        Vector3<float> f = front.normalize();
        Vector3<float> r = front.cross(top).normalize();
        Vector3<float> u = r.cross(f);
        mat4f r_m = Mat<float>::Identity(4);
        r_m.set_row(0, { r, 0 });
        r_m.set_row(1, { u, 0 });
        r_m.set_row(2, { -f, 0 });
        return r_m.mul(t_m);
    }
    static mat4f orthographic(float left, float right, float top, float bottom, float near, float far)
    {
        auto t_m = translate_matrix(
            { (left + right) / -2, (top + bottom) / -2, (near + far) / -2 });
        auto s_m = scale_matrix(
            { 2.0f / (right - left), 2.0f / (top - bottom), 2.0f / (near - far) });

        return s_m.mul(t_m);
    }
    static mat4f perspective(float fov, float aspect, float near, float far)
    {
        const float wei = 3.14f / 180;

        auto tan_half = std::tan(wei * fov / 2.0f);
        mat4f res { 0 };
        res.set(0, 0, 1.0f / (aspect * tan_half));
        res.set(1, 1, 1.0f / tan_half);
        res.set(2, 2, (-far - near) / (far - near));
        res.set(2, 3, (-2.0f * far * near) / (far - near));
        res.set(3, 2, -1.0f);
        return res;
    }
    static mat4f perspective(float left, float right, float top, float bottom, float near, float far)
    {
        assert(near > far && near < 0.0f && far < 0.0f);
        mat4f persp_to_ortho = mat4f::Identity(4);
        persp_to_ortho.set(0, 0, near);
        persp_to_ortho.set(1, 1, near);
        persp_to_ortho.set(3, 2, 1);
        persp_to_ortho.set_row(2, { 0, 0, near + far, -near * far });
        persp_to_ortho.set(3, 3, 0);
        auto orr = orthographic(left, right, top, bottom, near, far);
        return orthographic(left, right, top, bottom, near, far).mul(persp_to_ortho);
    }
    static mat4f screen_matrix(const int width, const int height)
    {
        mat4f res { 1 };
        res.set(0, 0, width / 2.0f);
        res.set(1, 1, height / 2.0f);
        res.set(2, 2, 1 / 2.0f);

        res.set_column(3, { width / 2.0f, height / 2.0f, 1 / 2.0f, 1 });

        return res;
    }
};
