#pragma once

#include <cmath>
#include <cstring>
#include <algorithm>

namespace gamebase {

class Matrix2 {
public:
    Matrix2()
    {
        m_matrix[0] = 1; m_matrix[2] = 0;
        m_matrix[1] = 0; m_matrix[3] = 1;
    }

    Matrix2(float m00, float m01, float m10, float m11)
    {
        m_matrix[0] = m00; m_matrix[2] = m01;
        m_matrix[1] = m10; m_matrix[3] = m11;
    }

    Matrix2(const Matrix2& other)
    {
        memcpy(m_matrix, other.m_matrix, sizeof(m_matrix));
    }

    Matrix2& operator=(const Matrix2& other)
    {
        memcpy(m_matrix, other.m_matrix, sizeof(m_matrix));
        return *this;
    }

    void set(size_t line, size_t col, float value)
    {
        m_matrix[col * 2 + line] = value;
    }

    float get(size_t line, size_t col) const
    {
        return m_matrix[col * 2 + line];
    }

    const float* dataPtr() const { return m_matrix; }
    float* dataPtr() { return m_matrix; }

    float determinant() const
    {
        return m_matrix[0] * m_matrix[3] - m_matrix[1] * m_matrix[2];
    }

    Matrix2& transpose()
    {
        std::swap(m_matrix[1], m_matrix[2]);
        return *this;
    }

    Matrix2 transposed() const
    {
        return Matrix2(
            m_matrix[0], m_matrix[1],
            m_matrix[2], m_matrix[3]);
    }

    float scaleX() const
    {
        return std::sqrtf(m_matrix[0] * m_matrix[0] + m_matrix[2] * m_matrix[2]);
    }

    float scaleY() const
    {
        return std::sqrtf(m_matrix[1] * m_matrix[1] + m_matrix[3] * m_matrix[3]);
    }

    Matrix2& operator+=(const Matrix2& other)
    {
        for (size_t i = 0; i < 4; ++i)
            m_matrix[i] += other.m_matrix[i];
        return *this;
    }

    Matrix2& operator-=(const Matrix2& other)
    {
        for (size_t i = 0; i < 4; ++i)
            m_matrix[i] -= other.m_matrix[i];
        return *this;
    }

    Matrix2& operator*=(const Matrix2& other)
    {
        Matrix2 m(*this);
        m_matrix[0] = m.m_matrix[0] * other.m_matrix[0] + m.m_matrix[2] * other.m_matrix[1];
        m_matrix[1] = m.m_matrix[1] * other.m_matrix[0] + m.m_matrix[3] * other.m_matrix[1];
        m_matrix[2] = m.m_matrix[0] * other.m_matrix[2] + m.m_matrix[2] * other.m_matrix[3];
        m_matrix[3] = m.m_matrix[1] * other.m_matrix[2] + m.m_matrix[3] * other.m_matrix[3];
        return *this;
    }

    Matrix2 operator-()
    {
        Matrix2 result;
        for (size_t i = 0; i < 4; ++i)
            result.m_matrix[i] = -m_matrix[i];
        return result;
    }

private:
    float m_matrix[4];
};

inline Matrix2 operator+(const Matrix2& m1, const Matrix2& m2)
{
    Matrix2 result(m1);
    result += m2;
    return result;
}

inline Matrix2 operator-(const Matrix2& m1, const Matrix2& m2)
{
    Matrix2 result(m1);
    result -= m2;
    return result;
}

inline Matrix2 operator*(const Matrix2& m1, const Matrix2& m2)
{
    Matrix2 result(m1);
    result *= m2;
    return result;
}


class ScalingMatrix2 : public Matrix2 {
public:
    ScalingMatrix2(float scale)
        : Matrix2(scale, 0, 0, scale)
    {}

    ScalingMatrix2(float scaleX, float scaleY)
        : Matrix2(scaleX, 0, 0, scaleY)
    {}
};


class RotationMatrix2 : public Matrix2 {
public:
    RotationMatrix2(float angle)
        : Matrix2(
            cosf(angle), -sinf(angle),
            sinf(angle),  cosf(angle))
    {}
};

}
