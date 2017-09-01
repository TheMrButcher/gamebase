/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include "Font.h"
#include "FontMetaData.h"
#include <gamebase/impl/serial/JsonSerializer.h>
#include <gamebase/impl/serial/JsonDeserializer.h>
#include <gamebase/tools/FileIO.h>
#include <locale>

namespace gamebase { namespace impl {
namespace {
#pragma pack(push, 1)
struct BFFHeader {
    unsigned char bffID[2];
    unsigned int imageWidth;
    unsigned int imageHeight;
    unsigned int cellWidth;
    unsigned int cellHeight;
    unsigned char bitsPerPixel;
    unsigned char firstSymbol;
};
#pragma pack(pop)

static_assert(sizeof(BFFHeader) == 20, "bad size of BFF header");

const unsigned int MIN_FONT_HEIGHT = 10;
const unsigned int MAX_FONT_HEIGHT = 256;

std::string extractFontName(const std::string& fname)
{
    size_t lastSpaceIndex = fname.find_last_of(" _");
    if (lastSpaceIndex == std::string::npos)
        THROW_EX() << "Can't extract font name. Font's file name has incorrect format: " << fname;
    size_t lastSlashIndex = fname.find_last_of("\\/");
    if (lastSlashIndex != std::string::npos && lastSlashIndex > lastSpaceIndex)
        THROW_EX() << "Can't extract font name. Unexpected slash after font's name: " << fname;
    size_t start = lastSlashIndex == std::string::npos ? 0 : lastSlashIndex + 1;
    return fname.substr(start, lastSpaceIndex - start);
}

unsigned int extractFontSize(const std::string& fname)
{
    static const std::locale LOCALE;
    std::string nameWithoutExtension = fname.substr(0, fname.find_last_of("."));
    size_t lastSpaceIndex = nameWithoutExtension.find_last_of(" _");
    if (lastSpaceIndex == std::string::npos
        || lastSpaceIndex == nameWithoutExtension.size() - 1
        || !std::isdigit(nameWithoutExtension[lastSpaceIndex + 1], LOCALE))
        THROW_EX() << "Font's file name has incorrect format: " << fname;
    unsigned int result = 0;
    std::stringstream ss(nameWithoutExtension.substr(lastSpaceIndex + 1));
    ss >> result;
    if (result < MIN_FONT_HEIGHT || result > MAX_FONT_HEIGHT)
        THROW_EX() << "Font " << nameWithoutExtension
            << " has incorrect size: " << result
            << ". Must be in [" << MIN_FONT_HEIGHT << "; " << MAX_FONT_HEIGHT << "]";
    return result;
}
}

Font::Font(const std::string& fontFileName, const std::string& metadataFileName)
    : m_name(extractFontName(fontFileName))
    , m_fileName(fontFileName)
    , m_metaDataFileName(metadataFileName)
    , m_fontSize(extractFontSize(fontFileName))
{}

void Font::load()
{
    auto data = loadBinaryFile(m_fileName);
    auto& header = reinterpret_cast<BFFHeader&>(data.front());
    if (header.bffID[0] != 0xBF || header.bffID[1] != 0xF2)
        THROW_EX() << "Not a BFF-file, incorrect ID";
    size_t imageSize = header.imageWidth * header.imageHeight * header.bitsPerPixel / 8;
    if (data.size() != sizeof(BFFHeader) + sizeof(m_glyphWidths) + imageSize)
        THROW_EX() << "Incorrect size of BFF-file";
    if (header.bitsPerPixel != 32)
        THROW_EX() << "BBF loader supports only 32 bits per pixel";

    m_cellSize = Size(header.cellWidth, header.cellHeight).toVector();
    m_glyphTextureSize = Vec2(
        float(header.cellWidth) / header.imageWidth,
        float(header.cellHeight) / header.imageHeight);
    m_glyphsPerLine = header.imageWidth / header.cellWidth;
    m_minIndex = header.firstSymbol;
    m_glyphsNum = static_cast<size_t>(std::min(
        (header.imageHeight / header.cellHeight) * m_glyphsPerLine, 256u - m_minIndex));

    if (m_glyphsNum == 0)
        THROW_EX() << "Empty font";

    if (!fileExists(m_metaDataFileName)) {
        m_metaData = std::make_shared<FontMetaData>(
            m_minIndex, m_glyphsNum + m_minIndex - 1);
        serializeToJsonFile(m_metaData, SerializationMode::Compressed, m_metaDataFileName);
    } else {
        deserializeFromJsonFile(m_metaDataFileName, m_metaData);
    }

    memcpy(m_glyphWidths, (&data.front()) + sizeof(BFFHeader), sizeof(m_glyphWidths));
    std::vector<uint8_t> imageData(imageSize);
    memcpy(&imageData.front(), (&data.front()) + sizeof(BFFHeader) + sizeof(m_glyphWidths), imageSize);
    size_t pixelsNum = imageSize / 4;
    for (size_t i = 0; i < pixelsNum; ++i) {
        imageData[i * 4] = 255;
        imageData[i * 4 + 1] = 255;
        imageData[i * 4 + 2] = 255;
    }
    Image image(std::move(imageData), Size(header.imageWidth, header.imageHeight));
    m_glyphAtlas = GLTexture(image);
}

std::vector<size_t> Font::glyphIndices(const std::string& utfStr) const
{
    return m_metaData->glyphIndices(utfStr);
}

BoundingBox Font::glyphTextureRect(size_t glyphIndex) const
{
    if (glyphIndex >= m_glyphsNum)
        return BoundingBox();
    size_t lineIndex = glyphIndex / m_glyphsPerLine;
	size_t columnIndex = glyphIndex - lineIndex * m_glyphsPerLine;
    Vec2 leftBottom(m_glyphTextureSize.x * columnIndex, m_glyphTextureSize.y * lineIndex);
    return BoundingBox(leftBottom, leftBottom + m_glyphTextureSize);
}

} }
