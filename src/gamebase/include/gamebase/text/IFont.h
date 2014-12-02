#pragma once

#include <gamebase/graphics/Texture.h>
#include <gamebase/geom/BoundingBox.h>
#include <string>

namespace gamebase {

class IFont {
public:
    virtual ~IFont() {}

    virtual const std::string& familyName() const = 0;

    virtual float fontSize() const = 0;

    virtual Vec2 cellSize() const = 0;

    virtual const Texture& texture() const = 0;

    virtual float getWidth(char ch) const = 0;

    virtual BoundingBox glyphTextureRect(char ch) const = 0;
};

inline float getTextLength(const std::string& str, const IFont* font)
{
    float result = 0;
    for (auto it = str.begin(); it != str.end(); ++it)
        result += font->getWidth(*it);
    return result;
}

}
