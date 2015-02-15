#include <stdafx.h>
#include <gamebase/engine/DrawablePolyline.h>
#include <gamebase/graphics/LineProgram.h>

namespace gamebase {

void DrawablePolyline::loadResources()
{
    m_buffers = createPolylineBuffers(m_polyline, m_width);
}

void DrawablePolyline::drawAt(const Transform2& position) const
{
    const auto& program = lineProgram();
    program.transform = position;
    program.color = m_color;
    program.draw(m_buffers.vbo, m_buffers.ibo);
}

}
