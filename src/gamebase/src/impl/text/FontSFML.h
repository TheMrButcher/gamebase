/**
* Copyright (c) 2017 Slavnejshev Filipp
* This file is licensed under the terms of the MIT license.
*/

#pragma once

#include <gamebase/impl/text/IFont.h>

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
        bool bold,
        float outlineWidth);

    virtual const std::string& familyName() const override { return m_familyName; }

    virtual float fontSize() const override { return static_cast<float>(m_size); }

    virtual float capHeight() const override;

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
    bool m_bold;
    float m_outlineWidth;
};

} }