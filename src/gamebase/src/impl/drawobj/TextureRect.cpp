/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/drawobj/TextureRect.h>
#include <gamebase/impl/graphics/TextureProgram.h>

namespace gamebase { namespace impl {

void TextureRect::loadResources()
{
    m_buffers = createTextureRectBuffers(m_rect);
}

void TextureRect::drawAt(const Transform2& position) const
{
    if (m_color.a == 0)
        return;
    const TextureProgram& program = textureProgram();
    program.transform = position;
    program.texture = m_texture;
    program.color = m_color;
    program.draw(m_buffers.vbo, m_buffers.ibo);
}

void TextureRect::registerProperties(const std::string& prefix, PropertiesRegisterBuilder* builder)
{
    if (prefix.empty()) {
        builder->registerProperty("color", &m_color);
        builder->registerProperty("r", &m_color.r);
        builder->registerProperty("g", &m_color.g);
        builder->registerProperty("b", &m_color.b);
        builder->registerProperty("a", &m_color.a);
    } else {
        builder->registerColor(prefix + "Color", &m_color);
    }
}

} }
