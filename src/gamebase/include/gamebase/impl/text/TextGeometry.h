/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/text/Aligner.h>
#include <gamebase/impl/text/FontStorage.h>
#include <gamebase/impl/graphics/GLBuffers.h>

namespace gamebase { namespace impl {

struct CharPosition {
    CharPosition(const BoundingBox& position, uint32_t glyphIndex)
        : position(position), glyphIndex(glyphIndex)
    {}

    BoundingBox position;
    uint32_t glyphIndex;
};

GAMEBASE_API std::vector<CharPosition> createTextGeometry(
    const std::vector<AlignedString>& alignedText,
    const IFont* font);

GAMEBASE_API GLBuffers createTextGeometryBuffers(
    const std::vector<CharPosition>& textGeom,
    const IFont* font);

inline GLBuffers createTextGeometryBuffers(
    const std::string& text,
    const AlignProperties& alignProps,
    const BoundingBox& box)
{
    auto alignedText = alignText(text, alignProps, box);
    auto font = alignProps.font.get();
    auto textGeom = createTextGeometry(alignedText, font.get());
    return createTextGeometryBuffers(textGeom, font.get());
}

} }
