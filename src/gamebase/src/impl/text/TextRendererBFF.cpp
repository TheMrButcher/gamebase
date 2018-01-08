/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include "TextRendererBFF.h"
#include <gamebase/impl/graphics/TextureProgram.h>

namespace gamebase { namespace impl {

TextRendererBFF::TextRendererBFF(const IFont* font, const GLTexture& texture)
    : m_font(font)
    , m_texture(texture)
{}

void TextRendererBFF::load(const std::vector<CharPosition>& textGeom)
{
    m_buffers = createTextGeometryBuffers(textGeom, m_font);
}

bool TextRendererBFF::empty() const
{
    return m_buffers.empty() || m_color.a == 0;
}

void TextRendererBFF::render(const Transform2& pos)
{
    const TextureProgram& program = textureProgram();
    program.transform = pos;
    program.texture = m_texture;
    program.color = m_color;
    program.draw(m_buffers.vbo, m_buffers.ibo);
}

} }
