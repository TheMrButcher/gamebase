/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/text/FontDescSFML.h>
#include <gamebase/tools/FileDesc.h>
#include <locale>

namespace gamebase { namespace impl {

namespace {
boost::optional<FontDescSFML> tryDecodeFileName(const FileDesc& file, char delimiter)
{
    static std::locale LOCALE;
    size_t pos = file.fileName.find_last_of(delimiter);
    if (pos == std::string::npos)
        return boost::none;
    auto familyName = file.fileName.substr(0, pos);
    auto fontMods = file.fileName.substr(pos + 1);
    for (auto& ch : fontMods)
        ch = std::toupper(ch, LOCALE);
    if (fontMods == "REGULAR" || fontMods == "R")
        return FontDescSFML{ familyName, file.path, false, false };
    if (fontMods == "BOLD" || fontMods == "B")
        return FontDescSFML{ familyName, file.path, true, false };
    if (fontMods == "ITALIC" || fontMods == "I")
        return FontDescSFML{ familyName, file.path, false, true };
    if (fontMods == "BOLDITALIC" || fontMods == "BI")
        return FontDescSFML{ familyName, file.path, true, true };
    return boost::none;
}
}

FontDescSFML FontDescSFML::fromFile(const FileDesc& file)
{
    auto result = tryDecodeFileName(file, ' ');
    if (result)
        return *result;
    result = tryDecodeFileName(file, '-');
    if (result)
        return *result;
    return FontDescSFML{ file.fileName, file.path, false, false };
}

} }