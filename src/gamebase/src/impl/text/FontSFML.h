/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/text/IFont.h>
#include <boost/optional.hpp>

namespace sf {
class Font;
class Glyph;
}

namespace gamebase { namespace impl {

class FontSFML : public IFont {
public:
    FontSFML(
        const std::shared_ptr<sf::Font>& font,
        const std::string& familyName,
        unsigned int size,
        bool makeBold,
        bool makeItalic,
        float outlineWidth);

    uint32_t style() const { return m_style; }
    const std::shared_ptr<sf::Font>& font() const { return m_font; }
    unsigned int fontSizeInt() const { return m_size; }
    float outlineWidth() const { return m_outlineWidth; }
    float offsetY() const;

    virtual NormalizationForm expectedForm() const override { return NormalizationForm::C; }

    virtual std::shared_ptr<ITextRenderer> makeRenderer() const override;

    virtual const std::string& familyName() const override { return m_familyName; }

    virtual float fontSize() const override { return static_cast<float>(m_size); }

    virtual float capHeight() const override;

    virtual float descent() const override;

    virtual float lineSpacing() const override;

    virtual const GLTexture& texture() const override;

    virtual std::vector<uint32_t> glyphIndices(const std::string& utfStr) const override;

    virtual float advance(uint32_t glyphIndex) const override;

    virtual float kerning(uint32_t glyphIndex1, uint32_t glyphIndex2) const override;

    virtual BoundingBox bounds(uint32_t glyphIndex) const override;

    virtual BoundingBox glyphTextureRect(uint32_t glyphIndex) const override;

private:
    const sf::Glyph& glyph(uint32_t glyphIndex) const;

    std::shared_ptr<sf::Font> m_font;
    std::string m_familyName;
    unsigned int m_size;
    uint32_t m_style;
    float m_outlineWidth;
    mutable boost::optional<float> m_offsetY;
};

} }
