/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/graphics/GLColor.h>
#include <gamebase/impl/geom/BoundingBox.h>
#include <gamebase/math/Vector2.h>
#include <stddef.h>
#include <vector>

namespace gamebase { namespace impl {

class BatchBuilder {
public:
    BatchBuilder() {}

    void reserveForTextureRects(size_t n)
    {
        reserveForTextureRects(vertices, n);
    }

    void addTextureRect(
        const BoundingBox& rect,
        const Vec2& texBottomLeft,
        const Vec2& texTopRight)
    {
        short offset = static_cast<short>(vertices.size() / 4);
        addTextureRect(vertices, rect, texBottomLeft, texTopRight);
        indices.push_back(offset); indices.push_back(offset + 1); indices.push_back(offset + 2);
        indices.push_back(offset + 1); indices.push_back(offset + 2); indices.push_back(offset + 3);
    }

    static void reserveForTextureRects(
        std::vector<float>& vertices,
        size_t n)
    {
        vertices.reserve(n * 4 * 4);
    }

    static void addTextureRect(
        std::vector<float>& vertices,
        const BoundingBox& rect,
        const Vec2& texBottomLeft,
        const Vec2& texTopRight)
    {
        addVec2(vertices, rect.bottomLeft);                    addVec2(vertices, texBottomLeft);
        addVec2(vertices, rect.bottomLeft.x, rect.topRight.y); addVec2(vertices, texBottomLeft.x, texTopRight.y);
        addVec2(vertices, rect.topRight.x, rect.bottomLeft.y); addVec2(vertices, texTopRight.x, texBottomLeft.y);
        addVec2(vertices, rect.topRight);                      addVec2(vertices, texTopRight);
    }

    static void addVec2(std::vector<float>& vertices, const Vec2& v)
    {
        vertices.push_back(v.x);
        vertices.push_back(v.y);
    }

    static void addVec2(std::vector<float>& vertices, float x, float y)
    {
        vertices.push_back(x);
        vertices.push_back(y);
    }

    static void addColor(std::vector<float>& vertices, const GLColor& c)
    {
        vertices.push_back(c.r);
        vertices.push_back(c.g);
        vertices.push_back(c.b);
        vertices.push_back(c.a);
    }

    std::vector<float> vertices;
    std::vector<uint16_t> indices;
};

} }
