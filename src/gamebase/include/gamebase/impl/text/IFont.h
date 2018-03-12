/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/text/NormalizationForm.h>
#include <gamebase/impl/graphics/GLTexture.h>
#include <gamebase/impl/geom/BoundingBox.h>
#include <string>

namespace gamebase { namespace impl {

class ITextRenderer;

class IFont {
public:
    virtual ~IFont() {}

    virtual NormalizationForm expectedForm() const = 0;

    virtual std::shared_ptr<ITextRenderer> makeRenderer() const = 0;

    virtual const std::string& familyName() const = 0;

    virtual float fontSize() const = 0;

    virtual float ascent() const = 0;

    virtual float descent() const = 0;

    virtual float lineSpacing() const = 0;

    virtual const GLTexture& texture() const = 0;

    virtual std::vector<uint32_t> glyphIndices(const std::string& utfStr) const = 0; 

    virtual float advance(uint32_t glyphIndex) const = 0;

    virtual float kerning(uint32_t glyphIndex1, uint32_t glyphIndex2) const = 0;

    virtual BoundingBox bounds(uint32_t glyphIndex) const = 0;

    virtual BoundingBox glyphTextureRect(uint32_t glyphIndex) const = 0;
};

inline float getTextLength(const std::vector<uint32_t>& glyphIndices, const IFont* font)
{
    float result = 0;
    auto it = glyphIndices.begin();
    auto itEnd = glyphIndices.end();
    for (; it != itEnd; ++it) {
        result += font->advance(it[0]);
        if (it + 1 < itEnd)
            result += font->kerning(it[0], it[1]);
    }
    return result;
}

inline float getTextLength(const std::string& utf8str, const IFont* font)
{
    return getTextLength(font->glyphIndices(utf8str), font);
}

} }
