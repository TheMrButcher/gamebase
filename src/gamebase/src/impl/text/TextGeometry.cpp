/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/text/TextGeometry.h>

namespace gamebase { namespace impl {

std::vector<CharPosition> createTextGeometry(
    const std::vector<AlignedString>& alignedText,
    const IFont* font)
{
    std::vector<CharPosition> result;
    size_t size = 0;
    for (const auto& alignedString : alignedText)
        size += alignedString.glyphIndices.size();
    result.reserve(size);
    for (const auto& alignedString : alignedText) {
        Vec2 offset = alignedString.baseLineStart;
        if (!alignedString.glyphIndices.empty())
            offset.x -= font->bounds(alignedString.glyphIndices.front()).left();
        for (auto glyphIndex : alignedString.glyphIndices) {
            BoundingBox position = font->bounds(glyphIndex);
            position.move(offset);
            result.emplace_back(position, glyphIndex);
            offset.x += font->advance(glyphIndex);
        }
    }
    return result;
}

GLBuffers createTextGeometryBuffers(
    const std::vector<CharPosition>& textGeom,
    const IFont* font)
{
    std::vector<float> vertices;
    vertices.reserve(textGeom.size() * 16);
    std::vector<short> indices;
    indices.reserve(textGeom.size() * 6);
    short offset = 0;
    for (const auto& ch : textGeom) {
        const auto& pos = ch.position;
        auto texBox = font->glyphTextureRect(ch.glyphIndex);

        vertices.push_back(pos.bottomLeft.x); vertices.push_back(pos.bottomLeft.y);
        vertices.push_back(texBox.bottomLeft.x); vertices.push_back(texBox.topRight.y);
        
        vertices.push_back(pos.bottomLeft.x); vertices.push_back(pos.topRight.y);
        vertices.push_back(texBox.bottomLeft.x); vertices.push_back(texBox.bottomLeft.y);
        
        vertices.push_back(pos.topRight.x); vertices.push_back(pos.bottomLeft.y);
        vertices.push_back(texBox.topRight.x); vertices.push_back(texBox.topRight.y);
        
        vertices.push_back(pos.topRight.x); vertices.push_back(pos.topRight.y);
        vertices.push_back(texBox.topRight.x); vertices.push_back(texBox.bottomLeft.y);

        indices.push_back(offset + 0); indices.push_back(offset + 1); indices.push_back(offset + 2);
        indices.push_back(offset + 1); indices.push_back(offset + 2); indices.push_back(offset + 3);

        offset += 4;
    }
    return GLBuffers(VertexBuffer(vertices), IndexBuffer(indices));
}

} }
