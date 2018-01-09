/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include "FontBFF.h"
#include "TextRendererBFF.h"
#include <gamebase/impl/text/IFont.h>

namespace gamebase { namespace impl {

class ScaledFont : public IFont {
public:
    ScaledFont(const FontBFF* originFont, float fontSize)
        : m_originFont(originFont)
        , m_fontSize(fontSize)
        , m_scale(fontSize / originFont->fontSize())
    {}

    virtual NormalizationForm expectedForm() const override { return m_originFont->expectedForm(); }

    virtual std::shared_ptr<ITextRenderer> makeRenderer() const override
    {
        return std::make_shared<TextRendererBFF>(this, m_originFont->texture());
    }

    virtual const std::string& familyName() const override { return m_originFont->familyName(); }

    virtual float fontSize() const override { return m_fontSize; }

    virtual float capHeight() const override { return m_scale * m_originFont->capHeight(); }

    virtual float lineSpacing() const override
    {
        return m_scale * m_originFont->lineSpacing();
    }

    virtual const GLTexture& texture() const override { return m_originFont->texture(); }

    virtual std::vector<uint32_t> glyphIndices(const std::string& utfStr) const override
    {
        return m_originFont->glyphIndices(utfStr);
    }

    virtual float advance(uint32_t glyphIndex) const override
    {
        return m_scale * m_originFont->advance(glyphIndex);
    }

    virtual float kerning(uint32_t glyphIndex1, uint32_t glyphIndex2) const override
    {
        return m_scale * m_originFont->kerning(glyphIndex1, glyphIndex1);
    }

    virtual BoundingBox bounds(uint32_t glyphIndex) const override
    {
        auto result = m_originFont->bounds(glyphIndex);
        result.bottomLeft *= m_scale;
        result.topRight *= m_scale;
        return result;
    }

    virtual BoundingBox glyphTextureRect(size_t glyphIndex) const override
    {
        return m_originFont->glyphTextureRect(glyphIndex);
    }

private:
    const FontBFF* m_originFont;
    float m_fontSize;
    float m_scale;
};

} }
