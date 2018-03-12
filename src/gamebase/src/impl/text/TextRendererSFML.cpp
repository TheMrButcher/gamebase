/**
* Copyright (c) 2018 Slavnejshev Filipp
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
    , m_style(m_font->style())
{}

void TextRendererSFML::load(const std::vector<AlignedString>& alignedText)
{
    m_renderedText.clear();
    for (const auto& alignedString : alignedText) {
        m_renderedText.emplace_back(
            sf::String::fromUtf32(
                alignedString.glyphIndices.begin(), alignedString.glyphIndices.end()),
            *m_font->font(),
            m_font->fontSizeInt());
        auto& sfmlText = m_renderedText.back();
        sfmlText.setFillColor(m_fillColor);
        sfmlText.setOutlineColor(m_outlineColor);
        sfmlText.setOutlineThickness(m_font->outlineWidth());
        sfmlText.setStyle(m_style);

        sfmlText.setPosition(
            std::roundf(alignedString.bbox.bottomLeft.x - sfmlText.getLocalBounds().left),
            std::roundf(-alignedString.bbox.topRight.y + m_font->offsetY()));
    }
}

void TextRendererSFML::setColor(const GLColor& color)
{
    m_fillColor = makeColorSFML(color);
    for (auto& sfmlText : m_renderedText)
        sfmlText.setFillColor(m_fillColor);
}

void TextRendererSFML::setOutlineColor(const GLColor& color)
{
    m_outlineColor = makeColorSFML(color);
    for (auto& sfmlText : m_renderedText)
        sfmlText.setOutlineColor(m_outlineColor);
}

void TextRendererSFML::setUnderlined(bool enabled)
{
    int flag = sf::Text::Underlined;
    m_style = enabled ? (m_style | flag) : (m_style & ~flag);
    for (auto& sfmlText : m_renderedText)
        sfmlText.setStyle(m_style);
}

void TextRendererSFML::setLineThrough(bool enabled)
{
    int flag = sf::Text::StrikeThrough;
    m_style = enabled ? (m_style | flag) : (m_style & ~flag);
    for (auto& sfmlText : m_renderedText)
        sfmlText.setStyle(m_style);
}

bool TextRendererSFML::empty() const
{
    return m_renderedText.empty()
        || (m_fillColor.a == 0
            && (m_font->outlineWidth() == 0 || m_outlineColor.a == 0));
}

void TextRendererSFML::render(const Transform2& pos)
{
    const State& curState = state();
    Transform2 invTransform = ScalingTransform2(0.5f * curState.width, 0.5f * curState.height);
    auto demulPos = pos * invTransform;
    auto data = demulPos.matrix.dataPtr();

    sf::Transform transformSFML(
        data[0], -data[2], std::roundf(0.5f * curState.width + demulPos.offset.x),
        -data[1], data[3], std::roundf(0.5f * curState.height - demulPos.offset.y),
        0.f, 0.f, 1.f);

    auto window = app->window().getImpl();
    window->pushGLStates();
    for (const auto& sfmlText : m_renderedText)
        window->draw(sfmlText, transformSFML);
    window->popGLStates();
}

} }
