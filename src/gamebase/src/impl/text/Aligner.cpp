/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/text/Aligner.h>
#include <gamebase/impl/text/Conversion.h>
#include <gamebase/impl/text/FontStorage.h>

namespace gamebase { namespace impl {
namespace {
std::vector<std::vector<uint32_t>> splitTextToWords(
    const std::vector<uint32_t>& text, uint32_t space)
{
    std::vector<std::vector<uint32_t>> result;
    auto itBegin = text.begin();
    auto it = std::find(text.begin(), text.end(), space);
    while (it != text.end()) {
        result.push_back(std::vector<uint32_t>(itBegin, it));
        itBegin = it + 1;
        it = std::find(itBegin, text.end(), space);
    }

    result.push_back(std::vector<uint32_t>(itBegin, it));
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
    uint32_t space = font->glyphIndices(SPACE_STR).at(0);
    auto words = splitTextToWords(font->glyphIndices(text), space);
    float spaceWidth = font->advance(space);

    std::vector<Line> lines;
    Line curLine;
    bool isLineStart = true;
    for (const auto& word : words) {
        float wordLength = getTextLength(word, font);
        if (!curLine.glyphIndices.empty() && curLine.length + spaceWidth + wordLength > maxLineLength) {
            lines.push_back(std::move(curLine));
            curLine.clear();
            isLineStart = true;
        }

        if (!isLineStart) {
            curLine.glyphIndices.push_back(space);
            curLine.length += spaceWidth;
        }

        if (curLine.glyphIndices.empty()) {
            if (!word.empty())
                curLine.length -= font->bounds(word.front()).left();
        }
        curLine.glyphIndices.insert(curLine.glyphIndices.end(), word.begin(), word.end());
        curLine.length += wordLength;
        isLineStart = false;
    }
    if (!isLineStart)
        lines.push_back(std::move(curLine));

    return lines;
}
}

std::vector<AlignedString> alignText(
    std::string text, const AlignProperties& alignProps, const BoundingBox& box)
{
    auto font = alignProps.font.get();
    
    if (font->expectedForm() == NormalizationForm::C)
        text = normalizeUtf8(text);
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
    float capHeight = font->capHeight();
    float lineSpacing = font->lineSpacing();
    float totalHeight = (lines.size() - 1) * lineSpacing + capHeight;

    float startY = box.bottomLeft.y;
    switch (alignProps.vertAlign) {
        case VertAlign::Top: startY += box.height(); break;
        case VertAlign::Center: startY += 0.5f * (box.height() + totalHeight); break;
        case VertAlign::Bottom: startY += totalHeight; break;
        default: THROW_EX() << "Bad VertAlign::Enum value: " << static_cast<int>(alignProps.vertAlign);
    }

    std::vector<AlignedString> result;
    result.reserve(lines.size());
    startY -= capHeight;
    for (auto it = lines.begin(); it != lines.end(); ++it, startY -= lineSpacing) {
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
        BoundingBox lineBox(start, start + Vec2(lineLength, capHeight));
        result.emplace_back(AlignedString(lineBox, std::move(it->glyphIndices)));
    }
    return result;
}

} }
