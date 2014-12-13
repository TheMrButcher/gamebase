#include <stdafx.h>
#include <gamebase/text/Aligner.h>
#include <gamebase/text/FontStorage.h>

namespace gamebase {
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
        : text(std::move(other.text))
        , length(other.length)
    {}

    Line& operator=(Line&& other)
    {
        text = std::move(other.text);
        length = other.length;
        return *this;
    }

    void clear()
    {
        text.clear();
        length = 0;
    }

    std::string text;
    float length;
};

std::vector<Line> splitTextToLines(
    const std::string& text,
    const IFont* font,
    float maxLineLength)
{
    auto words = splitTextToWords(text);
    float spaceWidth = font->getWidth(' ');

    std::vector<Line> lines;
    Line curLine;
    bool isLineStart = true;
    for (auto it = words.begin(); it != words.end(); ++it) {
        const auto& word = *it;
        float wordLength = getTextLength(word, font);
        if (!curLine.text.empty() && curLine.length + spaceWidth + wordLength > maxLineLength) {
            lines.push_back(std::move(curLine));
            curLine.clear();
            isLineStart = true;
        }

        if (!isLineStart) {
            curLine.text += ' ';
            curLine.length += spaceWidth;
        }

        curLine.text += word;
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
        line.text = text;
        line.length = getTextLength(text, font.get());
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
        Vec2 start(startX, startY);
        BoundingBox lineBox(start, start + Vec2(lineLength, fontSize)); 
        result.emplace_back(AlignedString(lineBox, std::move(it->text)));
    }
    return result;
}

}
