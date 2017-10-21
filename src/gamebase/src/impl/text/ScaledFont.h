/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/text/IFont.h>

namespace gamebase { namespace impl {

class ScaledFont : public IFont {
public:
    ScaledFont(const IFont* originFont, float fontSize)
        : m_originFont(originFont)
        , m_fontSize(fontSize)
        , m_scale(fontSize / originFont->fontSize())
    {}

    virtual const std::string& familyName() const override { return m_originFont->familyName(); }

    virtual float fontSize() const override { return m_fontSize; }

    virtual Vec2 cellSize() const override { return m_originFont->cellSize() * m_scale; }

    virtual const GLTexture& texture() const override { return m_originFont->texture(); }

    virtual std::vector<size_t> glyphIndices(const std::string& utfStr) const override
    {
        return m_originFont->glyphIndices(utfStr);
    }

    virtual float getWidth(size_t glyphIndex) const override
    {
        return m_scale * m_originFont->getWidth(glyphIndex);
    }

    virtual BoundingBox glyphTextureRect(size_t glyphIndex) const override
    {
        return m_originFont->glyphTextureRect(glyphIndex);
    }

private:
    const IFont* m_originFont; 
    float m_fontSize;
    float m_scale;
};

} }
