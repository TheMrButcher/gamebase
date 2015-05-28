#include <stdafx.h>
#include <gamebase/text/Conversion.h>
#include "ConversionInternal.h"
#include <gamebase/utils/Exception.h>

namespace gamebase {
std::string convertToUtf8(const std::string& localStr)
{
    int size = MultiByteToWideChar(
        CP_ACP, MB_COMPOSITE, localStr.c_str(),
        localStr.length(), nullptr, 0);
    std::wstring utf16Str(size, '\0');
    MultiByteToWideChar(
        CP_ACP, MB_COMPOSITE, localStr.c_str(),
        localStr.length(), &utf16Str[0], size);

    int utf8Size = WideCharToMultiByte(
        CP_UTF8, 0, utf16Str.c_str(), utf16Str.length(),
        nullptr, 0, nullptr, nullptr);
    std::string utf8Str(utf8Size, '\0');
    WideCharToMultiByte(
        CP_UTF8, 0, utf16Str.c_str(), utf16Str.length(),
        &utf8Str[0], utf8Size, nullptr, nullptr);
    return utf8Str;
}

size_t utf8CharLen(char c)
{
    if      (c >= 0 && c <= 127) return 1;
    else if ((c & 0xE0) == 0xC0) return 2;
    else if ((c & 0xF0) == 0xE0) return 3;
    else if ((c & 0xF8) == 0xF0) return 4;
    return 0;
}

std::vector<std::string> splitToUtf8Chars(const std::string& utf8Str)
{
    std::vector<std::string> result;
    auto it = utf8Str.begin();
    auto itEnd = utf8Str.end();
    while (it != itEnd) {
        auto len = utf8CharLen(*it);
        if (len == 0)
            THROW_EX() << "Can't determine length of utf8 character";
        if (itEnd - it < static_cast<ptrdiff_t>(len))
            THROW_EX() << "Detected invalid utf8 sequence";
        auto itNext = it + len;
        result.push_back(std::string(it, itNext));
        it = itNext;
    }
    return result;
}

}
