/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include "FontSFML.h"
#include <gamebase/tools/Exception.h>
#include <gamebase/math/Math.h>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/String.hpp>

namespace gamebase { namespace impl {

FontSFML::FontSFML(
    const std::shared_ptr<sf::Font>& font,
    const std::string& familyName,
    unsigned int size,
    bool bold,
    float outlineWidth)
    : m_font(font)
    , m_familyName(familyName)
    , m_size(size)
    , m_bold(bold)
    , m_outlineWidth(outlineWidth)
{}

float FontSFML::capHeight() const
{
    auto glyph = this->glyph('H');
    if (glyph.bounds.height <= 0)
        return static_cast<float>(m_size);
    return glyph.bounds.height;
}

float FontSFML::lineSpacing() const
{
    return m_font->getLineSpacing(m_size);
}

const GLTexture& FontSFML::texture() const
{
    THROW_EX() << "FontSFML::texture is not supported";
}

std::vector<uint32_t> FontSFML::glyphIndices(const std::string& utfStr) const
{
    auto sfmlStr = sf::String::fromUtf8(utfStr.begin(), utfStr.end());
    auto utf32Str = sfmlStr.toUtf32();
    return std::vector<uint32_t>(utf32Str.begin(), utf32Str.end());
}

float FontSFML::advance(uint32_t glyphIndex) const
{
    return glyph(glyphIndex).advance;
}

float FontSFML::kerning(uint32_t glyphIndex1, uint32_t glyphIndex2) const
{
    return m_font->getKerning(glyphIndex1, glyphIndex2, m_size);
}

BoundingBox FontSFML::bounds(uint32_t glyphIndex) const
{
    const auto& glyph = this->glyph(glyphIndex);
    return BoundingBox(
        Vec2(glyph.bounds.left, glyph.bounds.top),
        Vec2(glyph.bounds.left + glyph.bounds.width, glyph.bounds.top + glyph.bounds.height));
}

BoundingBox FontSFML::glyphTextureRect(uint32_t glyphIndex) const
{
    THROW_EX() << "FontSFML::glyphTextureRect(uint32_t) is not supported";
}

const sf::Glyph& FontSFML::glyph(uint32_t glyphIndex) const
{
    return m_font->getGlyph(glyphIndex, m_size, m_bold, m_outlineWidth);
}

} }
