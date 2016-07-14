/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include "FontMetaData.h"
#include "ConversionInternal.h"
#include <gamebase/impl/serial/ISerializer.h>
#include <gamebase/impl/serial/IDeserializer.h>

namespace gamebase { namespace impl {
namespace {
uint64_t toUInt64(std::string::const_iterator& it, std::string::const_iterator itEnd)
{
    auto len = utf8CharLen(it, itEnd);
    if (len == 0)
        THROW_EX() << "Can't determine length of utf8 character";
    if (itEnd - it < static_cast<ptrdiff_t>(len))
        THROW_EX() << "Detected invalid utf8 sequence";
    auto itCharEnd = it + len;
    uint64_t code = 0;
    for (; it != itCharEnd; ++it)
        code = ((code << 8) | static_cast<unsigned char>(*it));
    return code;
}
}

FontMetaData::FontMetaData() {}

FontMetaData::FontMetaData(int firstGlyph, int lastGlyph)
{
    size_t index = 0;
    for (int c = firstGlyph; c <= lastGlyph; ++c) {
        auto utf8Str = localToUtf8Code(static_cast<char>(c));
        auto code = toUInt64(utf8Str.cbegin(), utf8Str.cend());
        m_glyphIndices[code] = index++;
    }
}

std::vector<size_t> FontMetaData::glyphIndices(const std::string& utf8Str) const
{
    std::vector<size_t> result;
    auto itEnd = utf8Str.cend();
    for (auto it = utf8Str.cbegin(); it != itEnd;
        result.push_back(getGlyphIndex(toUInt64(it, itEnd))));
    return result;
}
        
void FontMetaData::serialize(Serializer& s) const
{
    s << "glyphIndices" << m_glyphIndices;
}

std::unique_ptr<IObject> deserializeFontMetaData(Deserializer& deserializer)
{
    std::unique_ptr<FontMetaData> result(new FontMetaData());
    deserializer >> "glyphIndices" >> result->m_glyphIndices;
    return std::move(result);
}

REGISTER_CLASS(FontMetaData);

size_t FontMetaData::getGlyphIndex(uint64_t utf8CharCode) const
{
    auto it = m_glyphIndices.find(utf8CharCode);
    if (it == m_glyphIndices.end())
        THROW_EX() << "Can't find glyph for char with utf8 code: " << utf8CharCode;
    return it->second;
}

} }
