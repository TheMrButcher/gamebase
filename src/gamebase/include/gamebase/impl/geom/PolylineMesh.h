/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/math/Vector2.h>
#include <vector>
#include <stddef.h>

namespace gamebase { namespace impl {

struct PolylineMesh {
    std::vector<float> vertices;
    std::vector<uint16_t> indices;
};

PolylineMesh GAMEBASE_API buildPolylineMesh(
    const Vec2* points, size_t size, float width);

inline PolylineMesh buildPolylineMesh(
    const std::vector<Vec2>& points, float width)
{
    return buildPolylineMesh(&points.front(), points.size(), width);
}

} }
