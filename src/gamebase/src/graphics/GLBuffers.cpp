#include <stdafx.h>
#include <gamebase/graphics/GLBuffers.h>

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

}
