#include <stdafx.h>
#include <gamebase/geom/Intersection.h>

namespace gamebase {

bool isPointInTriangle(const Vec2& point,
    const Vec2& p1, const Vec2& p2, const Vec2& p3)
{
    float a = cross(point - p1, p2 - p1);
    float b = cross(point - p2, p3 - p2);
    float c = cross(point - p3, p1 - p3);
    return (a >= 0 && b >= 0 && c >= 0) || (a <= 0 && b <= 0 && c <= 0);
}

}
