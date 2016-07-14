/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <cmath>
#include <cstring>
#include <algorithm>
#include <ostream>

namespace gamebase {

class Matrix2 {
public:
    Matrix2()
    {
        m_matrix[0] = 1.0f; m_matrix[2] = 0.0f;
        m_matrix[1] = 0.0f; m_matrix[3] = 1.0f;
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

    Matrix2& inverse()
    {
        float det = determinant();
        float tmp = m_matrix[0];
        m_matrix[0] = m_matrix[3] / det;
        m_matrix[3] = tmp / det;
        m_matrix[1] = -m_matrix[1] / det;
        m_matrix[2] = -m_matrix[2] / det;
        return *this;
    }

    Matrix2 inversed() const
    {
        float det = determinant();
        return Matrix2(
            m_matrix[3] / det, -m_matrix[2] / det,
            -m_matrix[1] / det, m_matrix[0] / det);
    }

    float scaleX() const
    {
        return std::sqrtf(m_matrix[0] * m_matrix[0] + m_matrix[1] * m_matrix[1]);
    }

    float scaleY() const
    {
        return std::sqrtf(m_matrix[2] * m_matrix[2] + m_matrix[3] * m_matrix[3]);
    }

    float angle() const
    {
        if (!hasRotation())
            return 0;
        float scale = scaleX();
        return atan2(m_matrix[1] / scale, m_matrix[0] / scale);
    }

    bool hasRotation() const
    {
        return m_matrix[1] || m_matrix[2];
    }

    bool isIdentityMatrix() const
    {
        return !hasRotation() && m_matrix[0] == m_matrix[3] && m_matrix[0] == 1.0f;
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

inline bool operator==(const Matrix2& m1, const Matrix2& m2)
{
    return memcmp(m1.dataPtr(), m2.dataPtr(), 4 * sizeof(float)) == 0;
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

inline std::ostream& operator<<(std::ostream& stream, const Matrix2& m)
{
    stream << "line0=(" << m.get(0, 0) << ", " << m.get(0, 1) << "), line1=("
                  << m.get(1, 0) << ", " << m.get(1, 1) << ")";
    return stream;
}

}
