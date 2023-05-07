#pragma once

#include "Helper/Math/Vec.hpp"
#include <string>
#include <string_view>

template <typename T>
class Mat {
public:
    Mat() = delete;
    ~Mat() {};
    Mat(Mat&& other) { data = std::move(other.data); }
    Mat(Mat& other) { data = other.data; }
    Mat(int _size, T value)
        : size(_size)
    {
        data.resize(size * size);
        for (int i = 0; i < size; i++) {
            set(i, i, value);
        }
    }
    void set(const int row, const int column, T v)
    {
        assert(data.size());
        data[get_size() * row + column] = v;
    }
    T get(const int row, const int column)
    {
        return data[row * get_size() + column];
    }
    void set_row(const int row, Vector<T> row_value)
    {
        assert(row_value.size == get_size());
        for (int i = 0; i < row_value.size; i++) {
            set(row, i, row_value[i]);
        }
    }
    Vector<T> get_row(const int row)
    {
        std::vector<T> res;
        for (int i = 0; i < get_size(); i++) {
            res.push_back(get(row, i));
        }
        return res;
    }
    void set_column(int column, Vector<T> column_value)
    {
        assert(column_value.size == size);
        for (int i = 0; i < column_value.size; i++) {
            set(i, column, column_value[i]);
        }
    }
    const Vector<T> get_column(int column)
    {
        std::vector<T> res;
        for (int i = 0; i < get_size(); i++) {
            res.push_back(get(i, column));
        }
        return res;
    }

    Vector<T> operator*(Vector<T> other)
    {
        assert(get_size() == other.size);
        std::vector<T> res;
        for (int i = 0; i < size; i++) {

            res.push_back(get_row(i).dot(other));
        }
        return res;
    }
    Mat<T> operator*(Mat<T> other)
    {
        assert(get_size() == other.get_size());
        Mat<T> res(size, data[0]);
        for (int i = 0; i < size; i++) {
            auto a = get_row(i);
            for (int j = 0; j < size; j++) {
                auto b = other.get_column(j);
                res.set(i, j, a.dot(b));
            }
        }
        return res;
    }
    // Vector<T> operator[](int index)
    // {
    //     return get_column(index);
    // }
    static Mat<T> Identity(int size)
    {
        return Mat<T>(size, 1.0f);
    }
    Mat<T> Transpose()
    {
        Mat<T> res(size, data[0]);
        for (int i = 0; i < size; i++) {
            res.set_row(i, get_column(i));
        }
        return res;
    }

    void scale(Vector<T> v)
    {
        assert(v.size == get_size() - 1);
    }

    [[nodiscard]] int get_size()
    {
        if (size == -1) {
            size = std::sqrt(data.size());
        }
        return size;
    }

private:
    std::vector<T> data;
    int size = -1;
};

template <typename T>
class Mat33 : public Mat<T> {
public:
    Mat33(T value)
        : Mat<T>(3, value)
    {
    }
    ~Mat33() { }
    Mat33(Mat<T>&& mat)
        : Mat<T>(mat)
    {
    }
    void set_row(const int row, Vector3<T> row_value)
    {
        Mat<T>::set_row(row, row_value);
    }
    Vector3<T> get_row(const int row) const
    {
        return Mat<T>::get_row(row);
    }
    void set_column(int column, Vector3<T> column_value)
    {
        Mat<T>::set_column(column, column_value);
    }
    const Vector3<T> get_column(int column) { return Mat<T>::get_column(column); }
    Vector3<T> mul(Vector3<T> other) { return (*this) * other; }
    Mat33<T> mul(Mat33<T> other) { return (*this) * other; }
};

template <typename T>
class Mat44 : public Mat<T> {
public:
    Mat44(T value)
        : Mat<T>(4, value)
    {
    }
    Mat44(Mat<T>&& mat)
        : Mat<T>(mat)
    {
    }
    // Vector4<T> operator[](int index)
    // {
    //     return get_column(index);
    // }
    void set_row(int row, Vector4<T> row_value)
    {
        Mat<T>::set_row(row, row_value);
    }
    Vector4<T> get_row(int row)
    {
        return Mat<T>::get_row(row);
    }
    void set_column(int column, Vector4<T> column_value)
    {
        Mat<T>::set_column(column, column_value);
    }
    Vector4<T> get_column(int column) { return Mat<T>::get_column(column); }
    Vector4<T> mul(Vector4<T> other) { return (*this) * other; }
    Mat44<T> mul(Mat44<T> other) { return (*this) * other; }
};
using mat4f = Mat44<float>;
