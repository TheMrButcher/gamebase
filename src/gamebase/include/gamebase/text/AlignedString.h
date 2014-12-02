#pragma once

#include <gamebase/geom/BoundingBox.h>
#include <string>
#include <vector>

namespace gamebase {

struct AlignedString {
    AlignedString(const BoundingBox& bbox, std::string&& text)
        : bbox(bbox), text(std::move(text))
    {}

    AlignedString(const BoundingBox& bbox, const std::string& text)
        : bbox(bbox), text(text)
    {}

    const Vec2& offset() const { return bbox.bottomLeft; }

    BoundingBox bbox;
    std::string text;
};

inline std::vector<AlignedString> placeText(const std::string& text, const Vec2& pos)
{
    std::vector<AlignedString> alignedText;
    alignedText.push_back(AlignedString(BoundingBox(pos), text));
    return alignedText;
}

}
