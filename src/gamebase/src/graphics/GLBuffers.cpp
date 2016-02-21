#include <stdafx.h>
#include <gamebase/graphics/GLBuffers.h>
#include <gamebase/geom/PolylineMesh.h>

namespace gamebase {

namespace {
static const short RECT_INDICES[6] = { 0, 1, 2, 1, 2, 3 };

inline void addVec2(std::vector<float>& data, const Vec2& v)
{
    data.push_back(v.x);
    data.push_back(v.y);
}

inline void addVec2(std::vector<float>& data, float x, float y)
{
    data.push_back(x);
    data.push_back(y);
}

inline void addColor(std::vector<float>& data, const Color& c)
{
    data.push_back(c.r);
    data.push_back(c.g);
    data.push_back(c.b);
    data.push_back(c.a);
}
}

GLBuffers createTriangleBuffers(
    float x0, float y0, float x1, float y1, float x2, float y2)
{
    static const short INDICES[3] = { 0, 1, 2 };
    std::vector<float> vertices;
    addVec2(vertices, x0, y0);
    addVec2(vertices, x1, y1);
    addVec2(vertices, x2, y2);
    return GLBuffers(VertexBuffer(vertices), IndexBuffer(INDICES, 3));
}

GLBuffers createTextureRectBuffers(
    const BoundingBox& rect,
    const Vec2& texBottomLeft,
    const Vec2& texTopRight)
{
    std::vector<float> vertices;
    addVec2(vertices, rect.bottomLeft);                    addVec2(vertices, texBottomLeft);
    addVec2(vertices, rect.bottomLeft.x, rect.topRight.y); addVec2(vertices, texBottomLeft.x, texTopRight.y);
    addVec2(vertices, rect.topRight.x, rect.bottomLeft.y); addVec2(vertices, texTopRight.x, texBottomLeft.y);
    addVec2(vertices, rect.topRight);                      addVec2(vertices, texTopRight);
    return GLBuffers(VertexBuffer(vertices), IndexBuffer(RECT_INDICES, 6));
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

GLBuffers createGradientBuffers(
    const BoundingBox& rect,
    const Color& color1,
    const Color& color2,
    Direction::Enum dir)
{
    std::vector<float> vertices;
    addVec2(vertices, rect.bottomLeft);                    addColor(vertices, color1);
    addVec2(vertices, rect.bottomLeft.x, rect.topRight.y); addColor(vertices, dir == Direction::Horizontal ? color1 : color2);
    addVec2(vertices, rect.topRight.x, rect.bottomLeft.y); addColor(vertices, dir == Direction::Horizontal ? color2 : color1);
    addVec2(vertices, rect.topRight);                      addColor(vertices, color2);
    return GLBuffers(VertexBuffer(vertices), IndexBuffer(RECT_INDICES, 6));
}

}
