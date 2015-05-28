#pragma once

#include <gamebase/geom/BoundingBox.h>
#include <vector>

namespace gamebase {

struct AlignedString {
    AlignedString(const BoundingBox& bbox, std::vector<size_t>&& glyphIndices)
        : bbox(bbox), glyphIndices(std::move(glyphIndices))
    {}

    AlignedString(const BoundingBox& bbox, const std::vector<size_t>& glyphIndices)
        : bbox(bbox), glyphIndices(glyphIndices)
    {}

    const Vec2& offset() const { return bbox.bottomLeft; }

    BoundingBox bbox;
    std::vector<size_t> glyphIndices;
};

}
