#include <stdafx.h>
#include <gamebase/graphics/GLBuffers.h>
#include <gamebase/geom/PolylineMesh.h>

namespace gamebase {

GLBuffers createTriangleBuffers(
    float x0, float y0, float x1, float y1, float x2, float y2)
{
    static const short INDICES[3] = { 0, 1, 2 };
    std::vector<float> vertices;
    vertices.push_back(x0); vertices.push_back(y0);
    vertices.push_back(x1); vertices.push_back(y1);
    vertices.push_back(x2); vertices.push_back(y2);
    return GLBuffers(VertexBuffer(vertices), IndexBuffer(INDICES, 3));
}

GLBuffers createTextureRectBuffers(
    const BoundingBox& rect,
    const Vec2& texBottomLeft,
    const Vec2& texTopRight)
{
    static const short INDICES[6] = { 0, 1, 2, 1, 2, 3 };

    std::vector<float> data;
    data.push_back(rect.bottomLeft.x); data.push_back(rect.bottomLeft.y); data.push_back(texBottomLeft.x); data.push_back(texBottomLeft.y);
    data.push_back(rect.bottomLeft.x); data.push_back(rect.topRight.y);   data.push_back(texBottomLeft.x); data.push_back(texTopRight.y);
    data.push_back(rect.topRight.x);   data.push_back(rect.bottomLeft.y); data.push_back(texTopRight.x); data.push_back(texBottomLeft.y);
    data.push_back(rect.topRight.x);   data.push_back(rect.topRight.y);   data.push_back(texTopRight.x); data.push_back(texTopRight.y);
    return GLBuffers(VertexBuffer(data), IndexBuffer(INDICES, 6));
}

GLBuffers createTextureRectBuffers(const BoundingBox& rect)
{
    static const Vec2 TEX_BOTTOM_LEFT(0, 1);
    static const Vec2 TEX_TOP_RIGHT(1, 0);
    return createTextureRectBuffers(rect, TEX_BOTTOM_LEFT, TEX_TOP_RIGHT);
}

GLBuffers createPolylineBuffers(const std::vector<Vec2>& points, float width)
{
    auto mesh = buildPolylineMesh(points, width);
    return GLBuffers(VertexBuffer(mesh.vertices), IndexBuffer(mesh.indices));
}

}
