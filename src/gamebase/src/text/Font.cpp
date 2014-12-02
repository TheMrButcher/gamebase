#include <stdafx.h>
#include "Font.h"
#include <gamebase/utils/FileIO.h>
#include <locale>

namespace gamebase {
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

Font::Font(const std::string& fileName)
    : m_name(extractFontName(fileName))
    , m_fileName(fileName)
    , m_fontSize(extractFontSize(fileName))
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
        THROW_EX() << "Loader supports only 32 bits per pixel";

    m_cellSize = Size(header.cellWidth, header.cellHeight).toVector();
    m_glyphTextureSize = Vec2(
        float(header.cellWidth) / header.imageWidth,
        float(header.cellHeight) / header.imageHeight);
    m_glyphsPerLine = header.imageWidth / header.cellWidth;
    m_minIndex = header.firstSymbol;
    m_maxIndex = static_cast<unsigned char>(std::min(
        m_minIndex + (header.imageHeight / header.cellHeight) * m_glyphsPerLine, 255u));

    memcpy(m_glyphWidths, (&data.front()) + sizeof(BFFHeader), sizeof(m_glyphWidths));
    std::vector<uint8_t> imageData(imageSize);
    memcpy(&imageData.front(), (&data.front()) + sizeof(BFFHeader) + sizeof(m_glyphWidths), imageSize);
    Image image(std::move(imageData), Size(header.imageWidth, header.imageHeight));
    m_glyphAtlas = Texture(image);
}

BoundingBox Font::glyphTextureRect(char ch) const
{
    auto index = static_cast<unsigned char>(ch);
    if (index < m_minIndex || index > m_maxIndex)
        return BoundingBox();
    index -= m_minIndex;
    unsigned char lineIndex = index / m_glyphsPerLine;
    unsigned char columnIndex = index - lineIndex * m_glyphsPerLine;
    Vec2 leftBottom(m_glyphTextureSize.x * columnIndex, m_glyphTextureSize.y * lineIndex);
    return BoundingBox(leftBottom, leftBottom + m_glyphTextureSize);
}

}
