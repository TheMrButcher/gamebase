/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/text/Conversion.h>
#include "ConversionInternal.h"
#include <gamebase/tools/Exception.h>
#include <unordered_map>
#include <unordered_set>

#include <iostream>

namespace gamebase { namespace impl {
namespace {
std::unordered_map<char, std::string> localToUtf8CodesMap;
std::unordered_set<std::string> utf8CodesSet;

size_t utf8CharLenInternal(char c)
{
    if      (c >= 0 && c <= 127) return 1;
    else if ((c & 0xE0) == 0xC0) return 2;
    else if ((c & 0xF0) == 0xE0) return 3;
    else if ((c & 0xF8) == 0xF0) return 4;
    return 0;
}
}

std::string convertToUtf8(const std::string& localStr)
{
    int size = MultiByteToWideChar(
        CP_ACP, MB_COMPOSITE, localStr.c_str(), localStr.length(), nullptr, 0);
    if (size == 0)
        return std::string();
    std::wstring utf16Str(size, '\0');
    if (!MultiByteToWideChar(
        CP_ACP, MB_COMPOSITE, localStr.c_str(), localStr.length(), &utf16Str[0], size))
        return std::string();

    int utf8Size = WideCharToMultiByte(
        CP_UTF8, 0, utf16Str.c_str(), utf16Str.length(),
        nullptr, 0, nullptr, nullptr);
    if (utf8Size == 0)
        return std::string();
    std::string utf8Str(utf8Size, '\0');
    if (!WideCharToMultiByte(
        CP_UTF8, 0, utf16Str.c_str(), utf16Str.length(),
        &utf8Str[0], utf8Size, nullptr, nullptr))
        return std::string();
    return std::move(utf8Str);
}

std::string convertToLocal(const std::string& utf8Str)
{
    int size = MultiByteToWideChar(
        CP_UTF8, 0, utf8Str.c_str(), utf8Str.length(), nullptr, 0);
    if (size == 0)
        return std::string();
    std::wstring utf16Str(size, '\0');
    if (!MultiByteToWideChar(
        CP_UTF8, 0, utf8Str.c_str(), utf8Str.length(), &utf16Str[0], size))
        return std::string();

    int localSize = WideCharToMultiByte(
        CP_ACP, WC_COMPOSITECHECK, utf16Str.c_str(), utf16Str.length(),
        nullptr, 0, " ", nullptr);
    if (localSize == 0)
        return std::string();
    std::string localStr(localSize, '\0');
    if (!WideCharToMultiByte(
        CP_ACP, WC_COMPOSITECHECK, utf16Str.c_str(), utf16Str.length(),
        &localStr[0], localSize, " ", nullptr))
        return std::string();
    return std::move(localStr);
}

size_t utf8CharLen(
    std::string::const_iterator it, std::string::const_iterator itEnd)
{
    auto len = utf8CharLenInternal(*it);
    if (len == 0)
        THROW_EX() << "Can't determine length of utf8 character";
    if (itEnd - it < static_cast<ptrdiff_t>(len))
        THROW_EX() << "Unexpected end of utf8 sequence";
    if (it + len == itEnd)
        return len;
    auto lenNext = utf8CharLenInternal(*(it + len));
    if (lenNext == 0)
        THROW_EX() << "Can't determine length of utf8 character";
    if (itEnd - it < static_cast<ptrdiff_t>(lenNext + len))
        THROW_EX() << "Unexpected end of utf8 sequence";
    if (utf8CodesSet.count(std::string(it, it + len + lenNext)) > 0)
        return len + lenNext;
    return len;
}

std::vector<std::string> splitToUtf8Chars(const std::string& utf8Str)
{
    std::vector<std::string> result;
    auto it = utf8Str.begin();
    auto itEnd = utf8Str.end();
    while (it != itEnd) {
        auto len = utf8CharLen(it, itEnd);
        auto itNext = it + len;
        result.push_back(std::string(it, itNext));
        it = itNext;
    }
    return result;
}

std::string localToUtf8Code(char c)
{
    auto it = localToUtf8CodesMap.find(c);
    if (it == localToUtf8CodesMap.end()) {
        std::string utf8Code = convertToUtf8(std::string(1, c));
        if (utf8Code.empty())
            return utf8Code;
        localToUtf8CodesMap[c] = utf8Code;
        utf8CodesSet.insert(utf8Code);

        size_t charsNum = 0;
        for (auto it = utf8Code.begin(); it != utf8Code.end();) {
            it += utf8CharLenInternal(*it);
            ++charsNum;
        }
        return utf8Code;
    }
    return it->second;
}

void initConversionMaps()
{
    for (int c = 0; c != 256; ++c) {
        try {
            localToUtf8Code(static_cast<char>(c));
        } catch (std::exception& ex) {
            std::cout << "Error during gerenation of utf8 code for local char with code " << c
                << ". Reason: " << ex.what() << std::endl;
        } catch (...) {
            std::cout << "Error during gerenation of utf8 code for local char with code " << c
                << ". Reason: unknown" << std::endl;
        }
    } 
}

} }
