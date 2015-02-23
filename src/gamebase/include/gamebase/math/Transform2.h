#pragma once

#include <gamebase/math/Matrix2.h>
#include <gamebase/math/Vector2.h>

namespace gamebase {

struct Transform2 {
    Transform2(const Matrix2& matrix = Matrix2(), const Vec2& offset = Vec2())
        : matrix(matrix)
        , offset(offset)
    {}

    Transform2 inversed() const
    {
        return Transform2(matrix.inversed(), -offset);
    }

    Transform2& operator*=(const Transform2& other)
    {
        matrix = other.matrix * matrix;
        offset = other.matrix * offset + other.offset;
        return *this;
    }

    Matrix2 matrix;
    Vec2 offset;
};

inline Transform2 operator*(const Transform2& t1, const Transform2& t2)
{
    Transform2 result(t1);
    result *= t2;
    return result;
}

inline Vec2 operator*(const Transform2& trans, const Vec2& v)
{
    return trans.matrix * v + trans.offset;
}

class ScalingTransform2 : public Transform2 {
public:
    ScalingTransform2(float scale)
        : Transform2(ScalingMatrix2(scale))
    {}

    ScalingTransform2(float scaleX, float scaleY)
        : Transform2(ScalingMatrix2(scaleX, scaleY))
    {}
};


class RotationTransform2 : public Transform2 {
public:
    RotationTransform2(float angle)
        : Transform2(RotationMatrix2(angle))
    {}
};


class ShiftTransform2 : public Transform2 {
public:
    ShiftTransform2(float x, float y)
        : Transform2(Matrix2(), Vec2(x, y))
    {}

    ShiftTransform2(const Vec2& offset)
        : Transform2(Matrix2(), offset)
    {}
};

inline std::ostream& operator<<(std::ostream& stream, const Transform2& t)
{
    stream << "matrix=[" << t.matrix << "], offset=" << t.offset;
    return stream;
}

}
