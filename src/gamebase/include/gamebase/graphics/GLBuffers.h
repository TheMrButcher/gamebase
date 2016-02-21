#pragma once

#include <gamebase/graphics/VertexBuffer.h>
#include <gamebase/graphics/IndexBuffer.h>
#include <gamebase/geom/BoundingBox.h>
#include <gamebase/math/Vector2.h>

namespace gamebase {

struct GLBuffers {
    GLBuffers() {}
    GLBuffers(const VertexBuffer& vbo, const IndexBuffer& ibo)
        : vbo(vbo)
        , ibo(ibo)
    {}

    VertexBuffer vbo;
    IndexBuffer ibo;
};

GAMEBASE_API GLBuffers createTriangleBuffers(
    float x0, float y0, float x1, float y1, float x2, float y2);

inline GLBuffers createTriangleBuffers(
    const Vec2& p0, const Vec2& p1, const Vec2& p2)
{
    return createTriangleBuffers(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);
}

inline GLBuffers createTriangleBuffers(const Vec2* points)
{
    return createTriangleBuffers(points[0], points[1], points[2]);
}

GAMEBASE_API GLBuffers createTextureRectBuffers(
    const BoundingBox& rect,
    const Vec2& texBottomLeft,
    const Vec2& texTopRight);

GAMEBASE_API GLBuffers createTextureRectBuffers(const BoundingBox& rect);

GAMEBASE_API GLBuffers createPolylineBuffers(
    const std::vector<Vec2>& points, float width);

}
