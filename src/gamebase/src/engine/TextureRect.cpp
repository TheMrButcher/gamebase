#include <stdafx.h>
#include <gamebase/engine/TextureRect.h>
#include <gamebase/graphics/TextureProgram.h>

namespace gamebase {

void TextureRect::loadResources()
{
    m_buffers = createTextureRectBuffers(m_rect);
}

void TextureRect::drawAt(const Transform2& position) const
{
    const TextureProgram& program = textureProgram();
    program.transform = position;
    program.texture = m_texture;
    program.color = m_color;
    program.draw(m_buffers.vbo, m_buffers.ibo);
}

}
