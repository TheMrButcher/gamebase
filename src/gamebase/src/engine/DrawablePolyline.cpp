#include <stdafx.h>
#include <gamebase/engine/DrawablePolyline.h>
#include <gamebase/graphics/LineProgram.h>

namespace gamebase {

void DrawablePolyline::loadResources()
{
    m_buffers = createPolylineBuffers(m_polyline, m_width);
}

void DrawablePolyline::draw(const Transform2& globalPosition) const
{
    const auto& program = lineProgram();
    program.transform = m_drawPosition
        ? m_drawPosition->transform() * globalPosition
        : globalPosition;
    program.color = m_color;
    program.draw(m_buffers.vbo, m_buffers.ibo);
}

}
