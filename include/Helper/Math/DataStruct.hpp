#pragma once
// #include "Helper/ConstData.hpp"
#include "Helper/Math/Vec.hpp"

struct Position3D : public Vector3<float> {

    Position3D()
        : Vector3<float> { 0, 0, 0 }

    {
    }
    Position3D(float _x, float _y, float _z)
        : Vector3<float> { _x, _y, _z }

    {
    }
    Position3D(Vector3<float> value)
        : Vector3<float> {
            value[0], value[1], value[2]
        }
    {
    }
};
struct Position2D : public Vector2<int> {

    Position2D()
        : Vector2<int> { 0, 0 }

    {
    }
    Position2D(int _x, int _y)
        : Vector2<int> { _x, _y }

    {
    }
    Position2D(Vector2<int> value)
        : Vector2<int> { value[0], value[1] }
    {
    }
    Position2D(Position3D v)
        : Vector2<int> { (int)v[0], (int)v[1] }
    {
    }
    bool operator<(Position2D other) const
    {
        return value[0] * 800 + value[1] < other.x() * 800 + other.y();
    }
};
struct Color : Vector4<uint8_t> {

    Color()
        : Vector4<uint8_t>()
    {
    }
    Color(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
        : Vector4<uint8_t> { _r, _g, _b, _a }

    {
    }
    Color(Vector<uint8_t> value)
        : Vector4<uint8_t> { value[0], value[1], value[2], value[3] }
    {
    }
    uint8_t& r()
    {
        return x();
    }
    uint8_t& g()
    {
        return y();
    }
    uint8_t& b()
    {
        return z();
    }
    uint8_t& a()
    {
        return w();
    }
};
struct UV : Vector2<float> {
    UV()
        : Vector2<float>()
    {
    }
    UV(float x, float y)
        : Vector2<float> { x, y }
    {
    }
    UV(Vector<float> value)
        : Vector2<float> { value }
    {
    }
    UV(Vector2<float> value)
        : Vector2<float> { value[0], value[1] }
    {
    }
};
struct Normal : Vector3<float> {
    Normal()
        : Vector3<float> { 0, 0, 0 }

    {
    }
    Normal(float _x, float _y, float _z)
        : Vector3<float> { _x, _y, _z }

    {
    }
    Normal(Vector<float> value)
        : Vector3<float> { value }
    {
    }
    Normal(Vector3<float> value)
        : Vector3<float> { value[0], value[1], value[2] }
    {
    }
};

struct Vertex {
    Vector4<float> pos;
    Vector4<float> color;
    UV uv;
    Normal normal;
    // float depth;
    Vertex operator*(float weight)
    {
        return Vertex {
            .pos { pos },
            .color { color * weight },
            .uv { uv * weight },
            .normal { normal * weight },
            // .depth = depth * weight
        };
    }
    Vertex operator+(Vertex other)
    {
        return Vertex {
            .pos { pos },
            .color { color + other.color },
            .uv { uv + other.uv },
            .normal { normal + other.normal },
            // .depth = depth + other.depth
        };
    }
};
struct Fragment {

    Vector2<int> pos;
    Vector4<float> color;
    UV uv;
    float depth;
    bool operator<(Fragment other) const
    {

        return pos.value[0] * 800 + pos.value[1] < other.pos.x() * 800 + other.pos.y();
    }
    // Fragment() = default;
    // Fragment(Vector2<int> pos, Color color, UV uv, float depth)
    //     : pos { pos }
    //     , color { color }
    //     , uv(uv)
    //     , depth { depth }
    // {
    // }
    // Fragment(Point3D point3d)
    //     : pos { (int)point3d.pos.x(), (int)point3d.pos.y() }
    //     , color { point3d.color }
    //     , uv(point3d.uv)
    //     , depth { point3d.pos.z() }
    // {
    // }
    // static Fragment lerp(Fragment a, Fragment b, Fragment c, float weight_a,
    //     float weight_b, float weight_c)
    // {
    //     //   return
    // }
    // Fragment operator*(float weight)
    // {
    //     return Fragment {   pos * weight, color * weight, uv * weight, depth * weight };
    // }
    // Fragment operator+(Fragment other)
    // {
    //     return Fragment { pos + other.pos, color + other.color, uv + other.uv, depth + other.depth };
    // }
};
struct Pixel {
    Position2D pos;
    Vector4<float> color;
};
struct RR {
    float r, g, b, a;
};