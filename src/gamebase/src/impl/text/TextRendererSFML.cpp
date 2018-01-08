/**
* Copyright (c) 2017 Slavnejshev Filipp
* This file is licensed under the terms of the MIT license.
*/

#include <stdafx.h>
#include "TextRendererSFML.h"
#include "FontSFML.h"
#include <gamebase/impl/app/Application.h>
#include <gamebase/math/Math.h>
#include <SFML/Graphics/RenderWindow.hpp>

#include "src/impl/graphics/State.h"

namespace gamebase { namespace impl {

namespace {
sf::Color makeColorSFML(const GLColor& color)
{
    auto color4i = color.intColor();
    return sf::Color(
        static_cast<uint8_t>(clamp(color4i.r, 0, 255)),
        static_cast<uint8_t>(clamp(color4i.g, 0, 255)),
        static_cast<uint8_t>(clamp(color4i.b, 0, 255)),
        static_cast<uint8_t>(clamp(color4i.a, 0, 255)));
}
}

TextRendererSFML::TextRendererSFML(const FontSFML* font)
    : m_font(font)
    , m_renderedText(sf::String(), *m_font->font(), m_font->fontSizeInt())
{
    m_renderedText.setOutlineThickness(m_font->outlineWidth());
    m_renderedText.setStyle(m_font->style());
}

void TextRendererSFML::load(const std::vector<CharPosition>& textGeom)
{
    std::vector<uint32_t> utf32;
    utf32.reserve(textGeom.size());
    for (auto ch : textGeom)
        utf32.push_back(ch.glyphIndex);
    m_renderedText.setString(sf::String::fromUtf32(utf32.begin(), utf32.end()));
}

void TextRendererSFML::setColor(const GLColor& color)
{
    m_renderedText.setFillColor(makeColorSFML(color));
}

void TextRendererSFML::setOutlineColor(const GLColor& color)
{
    m_renderedText.setOutlineColor(makeColorSFML(color));
}

void TextRendererSFML::setUnderlined(bool enabled)
{
    auto style = m_renderedText.getStyle();
    int flag = sf::Text::Underlined;
    style = enabled ? (style | flag) : (style & ~flag);
    m_renderedText.setStyle(style);
}

void TextRendererSFML::setLineThrough(bool enabled)
{
    auto style = m_renderedText.getStyle();
    int flag = sf::Text::StrikeThrough;
    style = enabled ? (style | flag) : (style & ~flag);
    m_renderedText.setStyle(style);
}

bool TextRendererSFML::empty() const
{
    return m_renderedText.getString().isEmpty()
        || (m_renderedText.getFillColor().a == 0
            && (m_renderedText.getOutlineThickness() == 0 || m_renderedText.getOutlineColor().a == 0));
}

void TextRendererSFML::render(const Transform2& pos)
{
    const State& curState = state();
    Transform2 invTransform = ScalingTransform2(0.5f * curState.width, -0.5f * curState.height);
    auto demulPos = pos * invTransform;
    auto data = demulPos.matrix.dataPtr();

    sf::Transform transformSFML(
        data[0], data[1], demulPos.offset.x,
        data[2], data[3], demulPos.offset.y,
        0.f, 0.f, 1.f);

    auto window = app->window().getImpl();
    window->pushGLStates();
    window->draw(m_renderedText, transformSFML);
    window->popGLStates();
}

} }
