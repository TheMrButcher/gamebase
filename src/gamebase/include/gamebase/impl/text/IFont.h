/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/graphics/GLTexture.h>
#include <gamebase/impl/geom/BoundingBox.h>
#include <string>

namespace gamebase { namespace impl {

class IFont {
public:
    virtual ~IFont() {}

    virtual const std::string& familyName() const = 0;

    virtual float fontSize() const = 0;

    virtual Vec2 cellSize() const = 0;

    virtual const GLTexture& texture() const = 0;

    virtual std::vector<size_t> glyphIndices(const std::string& utfStr) const = 0; 

    virtual float getWidth(size_t glyphIndex) const = 0;

    virtual BoundingBox glyphTextureRect(size_t glyphIndex) const = 0;
};

inline float getTextLength(const std::vector<size_t>& glyphIndices, const IFont* font)
{
    float result = 0;
    for (auto it = glyphIndices.begin(); it != glyphIndices.end(); ++it)
        result += font->getWidth(*it);
    return result;
}

inline float getTextLength(const std::string& utf8str, const IFont* font)
{
    return getTextLength(font->glyphIndices(utf8str), font);
}

} }
