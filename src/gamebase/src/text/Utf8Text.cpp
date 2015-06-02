#include <stdafx.h>
#include <gamebase/text/Utf8Text.h>
#include "ConversionInternal.h"
#include <sstream>

namespace gamebase { 
namespace {
void mergeUtf8Chars(
    std::ostringstream& oss,
    std::vector<std::string>::const_iterator it,
    std::vector<std::string>::const_iterator itEnd)
{
    for (; it != itEnd; ++it)
        oss << *it;
}

void mergeUtf8Chars(
    std::ostringstream& oss,
    const std::vector<std::string>& text,
    size_t pos, size_t count = size_t(-1))
{
    if (count == size_t(-1))
        count = text.size() - pos;
    auto itBegin = text.begin() + pos;
    mergeUtf8Chars(oss, itBegin, itBegin + count);
}
}

Utf8Text::Utf8Text(const std::string& text)
{
    set(text);
}

void Utf8Text::set(const std::string& text)
{
    m_utf8Chars = splitToUtf8Chars(text);
    m_text = text;
}

void Utf8Text::insert(size_t pos, const std::string& str)
{
    if (pos > m_utf8Chars.size())
        pos = m_utf8Chars.size();
    std::ostringstream oss;
    mergeUtf8Chars(oss, m_utf8Chars, 0, pos);
    oss << str;
    mergeUtf8Chars(oss, m_utf8Chars, pos);
    set(oss.str());
}

void Utf8Text::erase(size_t pos)
{
    erase(pos, pos + 1);
}

void Utf8Text::erase(size_t posBegin, size_t posEnd)
{
    if (posBegin >= posEnd)
        return;
    if (posBegin >= m_utf8Chars.size())
        return;
    if (posEnd > m_utf8Chars.size())
        posEnd = m_utf8Chars.size();
    std::ostringstream oss;
    mergeUtf8Chars(oss, m_utf8Chars, 0, posBegin);
    mergeUtf8Chars(oss, m_utf8Chars, posEnd);
    set(oss.str());
}

Utf8Text Utf8Text::substr(size_t pos, size_t count) const
{
    if (pos >= m_utf8Chars.size())
        return Utf8Text();
    if (count == size_t(-1) || count >= size() - pos)
        count = size() - pos;
    std::ostringstream oss;
    mergeUtf8Chars(oss, m_utf8Chars, pos, count);
    return Utf8Text(oss.str());
}

}
