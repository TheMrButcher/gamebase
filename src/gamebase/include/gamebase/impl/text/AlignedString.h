/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/geom/BoundingBox.h>
#include <vector>

namespace gamebase { namespace impl {

struct AlignedString {
    AlignedString() = default;
    AlignedString(
        const BoundingBox& bbox,
        const Vec2& baseLineStart,
        std::vector<uint32_t>&& glyphIndices)
        : bbox(bbox)
        , baseLineStart(baseLineStart)
        , glyphIndices(std::move(glyphIndices))
    {}

    BoundingBox bbox;
    Vec2 baseLineStart;
    std::vector<uint32_t> glyphIndices;
};

} }
