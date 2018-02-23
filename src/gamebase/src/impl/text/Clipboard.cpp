/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/text/Clipboard.h>
#include <gamebase/impl/text/Conversion.h>
#include <iostream>
#include <algorithm>

namespace gamebase { namespace impl {

std::string fromClipboardUtf8()
{
    return convertToUtf8(fromClipboardLocal());
}

std::string fromClipboardLocal()
{
    if (!OpenClipboard(0)) {
        std::cerr << "Error while opening clipboard. Error code: " << GetLastError() << std::endl;
        return std::string();
    }

    std::string result;
    HANDLE clipboardText = GetClipboardData(CF_TEXT);
    if (clipboardText) {
        char *text = reinterpret_cast<char*>(GlobalLock(clipboardText));
        result = text;
        GlobalUnlock(clipboardText);
    }
    CloseClipboard();

    // filter text
    std::replace(result.begin(), result.end(), '\t', ' ');
    std::replace(result.begin(), result.end(), '\n', ' ');
    result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
    
    return result;
}

void toClipboardUtf8(const std::string& utf8Str)
{
    toClipboardLocal(convertToLocal(utf8Str));
}

void toClipboardLocal(const std::string& localStr)
{
    const size_t len = localStr.length() + 1;
    HGLOBAL hMem =  GlobalAlloc(GMEM_MOVEABLE, len);
    memcpy(GlobalLock(hMem), localStr.c_str(), len);
    GlobalUnlock(hMem);
    if (!OpenClipboard(0)) {
        std::cerr << "Error while opening clipboard. Error code: " << GetLastError() << std::endl;
        return;
    }
    EmptyClipboard();
    if (!SetClipboardData(CF_TEXT, hMem)) {
        std::cerr << "Error while copying to clipboard. Error code: " << GetLastError() << std::endl;
        return;
    }
    CloseClipboard();
}

} }
