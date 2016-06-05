#include <stdafx.h>
#include <gamebase/impl/text/TextGeometry.h>

namespace gamebase { namespace impl {

std::vector<CharPosition> createTextGeometry(
    const std::vector<AlignedString>& alignedText,
    const IFont* font)
{
    Vec2 cellSize = font->cellSize();
    std::vector<CharPosition> result;
    for (auto it = alignedText.begin(); it != alignedText.end(); ++it) {
        Vec2 offset = it->offset();
        const auto& glyphIndices = it->glyphIndices;
        for (auto glyphIndexIt = glyphIndices.begin(); glyphIndexIt != glyphIndices.end(); ++glyphIndexIt) {
            size_t glyphIndex = *glyphIndexIt;
            BoundingBox position(offset, offset + cellSize);
            result.push_back(CharPosition(
                position, font->glyphTextureRect(glyphIndex)));
            offset.x += font->getWidth(glyphIndex);
        }
    }
    return result;
}

GLBuffers createTextGeometryBuffers(const std::vector<CharPosition>& textGeom)
{
    std::vector<float> vertices;
    vertices.reserve(textGeom.size() * 16);
    std::vector<short> indices;
    indices.reserve(textGeom.size() * 6);
    short offset = 0;
    for (auto it = textGeom.begin(); it != textGeom.end(); ++it) {
        const auto& pos = it->position;
        const auto& texBox = it->glyphTextureRect;

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
