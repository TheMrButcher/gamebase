#pragma once

#include <gamebase/text/Aligner.h>
#include <gamebase/text/FontStorage.h>
#include <gamebase/graphics/GLBuffers.h>

namespace gamebase {

struct CharPosition {
    CharPosition(const BoundingBox& position, const BoundingBox& glyphTextureRect)
        : position(position), glyphTextureRect(glyphTextureRect)
    {}

    BoundingBox position;
    BoundingBox glyphTextureRect;
};

GAMEBASE_API std::vector<CharPosition> createTextGeometry(
    const std::vector<AlignedString>& alignedText,
    const IFont* font);

GAMEBASE_API GLBuffers createTextGeometryBuffers(
    const std::vector<CharPosition>& textGeom);

inline GLBuffers createTextGeometryBuffers(
    const std::string& text,
    const AlignProperties& alignProps,
    const BoundingBox& box)
{
    auto alignedText = alignText(text, alignProps, box);
    auto font = alignProps.font.get();
    auto textGeom = createTextGeometry(alignedText, font.get());
    return createTextGeometryBuffers(textGeom);
}

}
