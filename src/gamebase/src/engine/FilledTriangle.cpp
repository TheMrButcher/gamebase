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

void FilledTriangle::draw(const Transform2& globalPosition) const
{
    const OneColorProgram& program = oneColorProgram();
    program.transform = m_drawPosition->transform() * globalPosition;
    program.color = m_color;
    program.draw(m_buffers.vbo, m_buffers.ibo);
}

}
