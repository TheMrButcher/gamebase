#include <stdafx.h>
#include <gamebase/engine/FilledTriangle.h>
#include <gamebase/graphics/OneColorProgram.h>
#include <cstring>

namespace gamebase {

void FilledTriangle::setPoints(const Vec2* points)
{
    memcpy(m_points, points, sizeof(m_points));
}

void FilledTriangle::loadResources()
{
    m_buffers = createTriangleBuffers(m_points);
}

void FilledTriangle::drawAt(const Transform2& position) const
{
    const OneColorProgram& program = oneColorProgram();
    program.transform = position;
    program.color = m_color;
    program.draw(m_buffers.vbo, m_buffers.ibo);
}

}
