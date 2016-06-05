#include <stdafx.h>
#include <gamebase/impl/text/Aligner.h>
#include <gamebase/impl/text/FontStorage.h>

namespace gamebase { namespace impl {
namespace {
std::vector<std::string> splitTextToWords(const std::string& text)
{
    std::vector<std::string> result;
    size_t curStart = 0;
    size_t nextDelim;
    while ((nextDelim = text.find(' ', curStart)) != std::string::npos) {
        //if (curStart < nextDelim)
            result.push_back(text.substr(curStart, nextDelim - curStart));
        curStart = nextDelim + 1;
    }

    //if (curStart != text.length())
        result.push_back(text.substr(curStart));
    return result;
}

struct Line {
    Line() { clear(); }
    Line(Line&& other)
        : glyphIndices(std::move(other.glyphIndices))
        , length(other.length)
    {}

    Line& operator=(Line&& other)
    {
        glyphIndices = std::move(other.glyphIndices);
        length = other.length;
        return *this;
    }

    void clear()
    {
        glyphIndices.clear();
        length = 0;
    }

    std::vector<size_t> glyphIndices;
    float length;
};

std::vector<Line> splitTextToLines(
    const std::string& text,
    const IFont* font,
    float maxLineLength)
{
    static const std::string SPACE_STR(1, ' ');
    auto words = splitTextToWords(text);

    size_t spaceGlyphIndex = font->glyphIndices(SPACE_STR).at(0);
    float spaceWidth = font->getWidth(spaceGlyphIndex);

    std::vector<Line> lines;
    Line curLine;
    bool isLineStart = true;
    for (auto it = words.begin(); it != words.end(); ++it) {
        const auto& word = *it;
        auto wordGlyphIndices = font->glyphIndices(word);
        float wordLength = getTextLength(wordGlyphIndices, font);
        if (!curLine.glyphIndices.empty() && curLine.length + spaceWidth + wordLength > maxLineLength) {
            lines.push_back(std::move(curLine));
            curLine.clear();
            isLineStart = true;
        }

        if (!isLineStart) {
            curLine.glyphIndices.push_back(spaceGlyphIndex);
            curLine.length += spaceWidth;
        }

        curLine.glyphIndices.insert(curLine.glyphIndices.end(),
            wordGlyphIndices.begin(), wordGlyphIndices.end());
        curLine.length += wordLength;
        isLineStart = false;
    }
    if (!isLineStart)
        lines.push_back(std::move(curLine));

    return lines;
}
}

std::vector<AlignedString> alignText(
    const std::string& text, const AlignProperties& alignProps, const BoundingBox& box)
{
    auto font = alignProps.font.get();
    std::vector<Line> lines;
    if (alignProps.enableStacking) {
        lines = splitTextToLines(text, font.get(), box.width());
    } else {
        Line line;
        line.glyphIndices = font->glyphIndices(text);
        line.length = getTextLength(line.glyphIndices, font.get());
        lines.push_back(std::move(line));
    }
    float maxLineLength = 0.0f;
    for (auto it = lines.begin(); it != lines.end(); ++it)
        maxLineLength = std::max(maxLineLength, it->length);
    float fontSize = alignProps.font.size;
    float totalHeight = lines.size() * fontSize;

    float startY = box.bottomLeft.y;
    switch (alignProps.vertAlign) {
        case VertAlign::Top: startY += box.height(); break;
        case VertAlign::Center: startY += 0.5f * (box.height() + totalHeight); break;
        case VertAlign::Bottom: startY += totalHeight; break;
        default: THROW_EX() << "Bad VertAlign::Enum value: " << static_cast<int>(alignProps.vertAlign);
    }

    std::vector<AlignedString> result;
    result.reserve(lines.size());
    startY -= fontSize;
    for (auto it = lines.begin(); it != lines.end(); ++it, startY -= fontSize) {
        float lineLength = it->length;
        float startX = box.bottomLeft.x;
        switch (alignProps.horAlign) {
            case HorAlign::Left: break;
            case HorAlign::Center: startX += 0.5f * (box.width() - lineLength); break;
            case HorAlign::Right: startX += box.width() - lineLength; break;
            default: THROW_EX() << "Bad HorAlign::Enum value: " << static_cast<int>(alignProps.horAlign);
        }
        startX = static_cast<float>(static_cast<int>(startX));
        Vec2 start(startX, startY);
        BoundingBox lineBox(start, start + Vec2(lineLength, fontSize)); 
        result.emplace_back(AlignedString(lineBox, std::move(it->glyphIndices)));
    }
    return result;
}

} }
