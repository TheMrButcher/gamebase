#include <stdafx.h>
#include <gamebase/engine/StaticLabel.h>
#include <gamebase/text/TextGeometry.h>
#include <gamebase/graphics/TextureProgram.h>

namespace gamebase {

void StaticLabel::loadResources()
{
    m_font = m_alignProps.font.get();
    m_buffers = createTextGeometryBuffers(
        m_text, m_alignProps, m_rect);
}

void StaticLabel::drawAt(const Transform2& position) const
{
    if (m_text.empty())
        return;
    const TextureProgram& program = textureProgram();
    program.transform = position;
    program.texture = m_font->texture();
    program.color = m_color;
    program.draw(m_buffers.vbo, m_buffers.ibo);
}

}
