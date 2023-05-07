#pragma once
#include <cassert>
#include <vector>

template <class T>
concept Check = requires(T a, T b) {
                    a + b;
                    a - b;
                    a* b;
                    a / b;
                };
template <Check T>
struct Vector {
public:
    std::vector<T> value;
    int size;

    Vector() = delete;
    Vector(std::vector<T> _value)
    {
        value = _value;

        size = value.size();
    }
    T& get(int index)
    {
        assert(index >= 0 && index < size);
        return value[index];
    }
    T& operator[](const int index)
    {
        assert(index >= 0 && index < size);
        return value[index];
    }
    bool operator==(Vector other)
    {
        assert(size == other.size);
        for (int i = 0; i < size; i++) {
            if (value[i] != other.value[i])
                return false;
        }
        return true;
    }

    Vector operator+(Vector other)
    {
        assert(size == other.size);
        std::vector<T> res;
        for (int i = 0; i < size; i++) {
            res.emplace_back(value[i] + other[i]);
        }
        return res;
    }

    Vector operator-(Vector other)
    {
        assert(size == other.size);
        std::vector<T> res;
        for (int i = 0; i < size; i++) {
            res.emplace_back(value[i] - other[i]);
        }
        return res;
    }
    Vector operator-()
    {

        std::vector<T> res;
        for (int i = 0; i < size; i++) {
            res.emplace_back(value[i] * T(-1));
        }
        return res;
    }

    Vector operator*(float weight)
    {
        std::vector<T> res;
        for (int i = 0; i < size; i++) {
            res.emplace_back(value[i] * weight);
        }
        return res;
    }
    friend Vector operator*(float weight, Vector a)
    {
        return a * weight;
    }
    Vector operator*(Vector other)
    {
        assert(size == other.size);
        std::vector<T> res;
        for (int i = 0; i < size; i++) {
            res.emplace_back(value[i] * other[i]);
        }
        return res;
    }

    Vector operator/(Vector other)
    {
        assert(size == other.size);
        std::vector<T> res;
        for (int i = 0; i < size; i++) {
            res.emplace_back(value[i] / other[i]);
        }
        return res;
    }
    Vector operator/(float weight)
    {

        std::vector<T> res;
        for (int i = 0; i < size; i++) {
            res.emplace_back(value[i] / weight);
        }
        return res;
    }

    Vector abs() { return std::abs(value); }

    T dot(Vector<T> b)
    {
        auto res = 0.0f;
        for (int i = 0; i < size; i++) {
            res += value[i] * b[i];
        }
        return res;
    }

    float square()
    {
        float res = 0;
        for (auto& i : value) {
            res += float(i * i);
        }
        return res;
    }
    float length() { return std::sqrt(square()); }
    Vector<T> normalize()
    {
        return *this / length();
    }
};
template <Check T>
struct Vector2 : public Vector<T> {
    Vector2()
        : Vector<T> { { (T)0, (T)0 } }
    {
    }
    Vector2(T _x, T _y)
        : Vector<T> { { _x, _y } }

    {
    }
    Vector2(Vector<T> value)
        : Vector<T> { value }

    {
        assert(value.size == 2);
    }

    T& x() { return this->value[0]; }
    T& y() { return this->value[1]; }
    T cross(Vector<T> b) { return this->value[0] * b[1] - this->value[1] * b[0]; }
};

template <typename T>
struct Vector3 : public Vector<T> {
    Vector3()
        : Vector<T> { { 0, 0, 0 } }
    {
    }
    Vector3(Vector<T> value)
        : Vector<T> { value }
    {
        assert(value.size == 3);
    }
    Vector3(T _x, T _y, T _z)
        : Vector<T> { { _x, _y, _z } }

    {
    }
    // Vector3(Vector<T>&& value)
    //     : Vector<T> { { value[0], value[1], value[2] } }

    // {
    // }

    T& x() { return this->value[0]; }
    T& y() { return this->value[1]; }
    T& z() { return this->value[2]; }
    Vector3<T> cross(Vector<T> b)
    {
        return {
            T(this->value[1] * b[2] - this->value[2] * b[1]),
            T(this->value[2] * b[0] - this->value[0] * b[2]),
            T(this->value[0] * b[1] - this->value[1] * b[0])
        };
    }
};
template <typename T>
struct Vector4 : public Vector<T> {
    Vector4()
        : Vector<T> { { 0, 0, 0, 0 } }
    {
    }

    Vector4(T _x, T _y, T _z, T _w)
        : Vector<T> { { (T)_x, (T)_y, (T)_z, (T)_w } }

    {
    }
    Vector4(Vector3<T> vec3, T v)
        : Vector<T> { { vec3[0], vec3[1], vec3[2], v } }
    {
    }
    Vector4(Vector<T> value)
        : Vector<T> { value }
    {
        assert(value.size == 4);
    }

    T& x() { return this->value[0]; }
    T& y() { return this->value[1]; }
    T& z() { return this->value[2]; }
    T& w() { return this->value[3]; }
    // Vector<T> cross(Vector<T> b)
    // {
    //     return {
    //         { T(this->value[1] * b[2] - this->value[2] * b[1]),
    //             T(this->value[2] * b[0] - this->value[0] * b[2]),
    //             T(this->value[0] * b[1] - this->value[1] * b[0]) }
    //     };
    // }
};
