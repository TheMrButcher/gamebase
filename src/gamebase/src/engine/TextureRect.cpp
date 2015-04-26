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

void TextureRect::registerProperties(const std::string& prefix, PropertiesRegisterBuilder* builder)
{
    builder->registerColor(prefix.empty() ? "color" : prefix + "Color", &m_color);
}

}
