/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/geom/BoundingBox.h>
#include <vector>

namespace gamebase { namespace impl {

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

} }
