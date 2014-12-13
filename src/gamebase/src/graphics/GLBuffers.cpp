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

GLBuffers createTextureRectBuffers(const BoundingBox& rect)
{
    static const short INDICES[6] = { 0, 1, 2, 1, 2, 3 };

    std::vector<float> data;
    data.push_back(rect.bottomLeft.x); data.push_back(rect.bottomLeft.y); data.push_back(0); data.push_back(0);
    data.push_back(rect.bottomLeft.x); data.push_back(rect.topRight.y);   data.push_back(0); data.push_back(1);
    data.push_back(rect.topRight.x);   data.push_back(rect.bottomLeft.y); data.push_back(1); data.push_back(0);
    data.push_back(rect.topRight.x);   data.push_back(rect.topRight.y);   data.push_back(1); data.push_back(1);
    return GLBuffers(VertexBuffer(data), IndexBuffer(INDICES, 6));
}

GLBuffers createPolylineBuffers(const std::vector<Vec2>& points, float width)
{
    auto mesh = buildPolylineMesh(points, width);
    return GLBuffers(VertexBuffer(mesh.vertices), IndexBuffer(mesh.indices));
}

}
