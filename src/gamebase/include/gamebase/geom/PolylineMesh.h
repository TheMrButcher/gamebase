#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/math/Vector2.h>
#include <vector>

namespace gamebase {

struct PolylineMesh {
    std::vector<float> vertices;
    std::vector<short> indices;
};

PolylineMesh GAMEBASE_API buildPolylineMesh(
    const Vec2* points, size_t size, float width);

inline PolylineMesh buildPolylineMesh(
    const std::vector<Vec2>& points, float width)
{
    return buildPolylineMesh(&points.front(), points.size(), width);
}

}
