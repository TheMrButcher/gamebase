/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/text/FontStorage.h>
#include "FontBFF.h"
#include "ScaledFont.h"
#include "src/impl/global/GlobalResources.h"
#include <gamebase/tools/FileIO.h>
#include <gamebase/math/Math.h>
#include <iostream>

namespace gamebase { namespace impl {
namespace {
const std::string DEFAULT_FONT("Arial");

bool compareFonts(const std::shared_ptr<IFont>& font1, const std::shared_ptr<IFont>& font2)
{
    return font1->fontSize() < font2->fontSize();
}

bool compareSizes(const std::shared_ptr<IFont>& font, float fontSize)
{
    return uround(font->fontSize()) < uround(fontSize);
}

bool loadFontBFF(FontBFF* font)
{
    if (font->isLoaded())
        return true;
    try {
        font->load();
        std::cout << "Loaded font: \"" << font->familyName() << "\", height: " << font->fontSize() << std::endl;
        return true;
    }
    catch (std::exception& ex) {
        std::cerr << "Can't load font from file: " << font->fileName()
            << ". Reason: " << ex.what() << std::endl;
        return false;
    }
}
}

FontStorage::FontStorage()
    : m_defaultFamilyName(DEFAULT_FONT)
{}

void FontStorage::load(const std::string& fontsPath)
{
    auto files = listFilesInDirectory(fontsPath);
    extractFontsBFF(files);
}

void FontStorage::prepare()
{
    m_fontNames.clear();
    m_fontNames.reserve(m_fontFamilies.size());
    for (auto it = m_fontFamilies.begin(); it != m_fontFamilies.end(); ++it) {
        auto& fonts = it->second;
        std::sort(fonts.begin(), fonts.end(), compareFonts);
        m_fontNames.push_back(it->first);
    }
    std::sort(m_fontNames.begin(), m_fontNames.end());

    if (m_fontFamilies.empty()) {
        std::cerr << "Warning! No fonts are detected" << std::endl;
        return;
    }

    auto it = m_fontFamilies.find(DEFAULT_FONT);
    if (it == m_fontFamilies.end())
        m_defaultFamilyName = m_fontNames.front();
}

std::shared_ptr<IFont> FontStorage::getFont(float fontSize) const
{
    return getFont(m_defaultFamilyName, fontSize);
}

std::shared_ptr<IFont> FontStorage::getFont(const std::string& familyName, float fontSize) const
{
    if (m_fontFamilies.empty())
        THROW_EX() << "Can't get any font. FontStorage is empty";
    if (fontSize <= 0)
        THROW_EX() << "Wrong size of font: " << fontSize;
    auto it = m_fontFamilies.find(familyName);
    if (it == m_fontFamilies.end())
        it = m_fontFamilies.find(m_defaultFamilyName);
    if (it == m_fontFamilies.end()) {
        std::cerr << "Can't find nor " << familyName << " font, neither default "
            << m_defaultFamilyName << " font" << std::endl;
        m_defaultFamilyName = m_fontFamilies.begin()->first;
        std::cerr << m_defaultFamilyName << " is now default font" << std::endl;
        it = m_fontFamilies.begin();
    }
    auto& fonts = it->second;
    auto font = findFontBFF(fonts, fontSize);
    if (!font) {
        if (fonts.empty())
            m_fontFamilies.erase(it);
        return getFont(familyName, fontSize);
    }
    return font;
}

void FontStorage::extractFontsBFF(const std::vector<FileDesc>& files)
{
    std::vector<std::string> fontFileNames;
    for (const auto& file : files) {
        if (file.type == FileDesc::File && file.extension == "bff")
            fontFileNames.push_back(file.path);
    }

    for (const auto& fileName : fontFileNames) {
        std::string metaDataFileName =
            fileName.substr(0, fileName.find_last_of(".")) + ".json";
        auto font = std::make_shared<FontBFF>(fileName, metaDataFileName);
        m_fontFamilies[font->familyName()].push_back(font);
    }
}

std::shared_ptr<IFont> FontStorage::findFontBFF(
    const std::vector<std::shared_ptr<FontBFF>>& fonts, float fontSize) const
{
    auto fontIt = std::lower_bound(fonts.begin(), fonts.end(), fontSize, compareSizes);
    FontBFF* originFont = nullptr;
    if (fontIt == fonts.end()) {
        originFont = fonts.back().get();
    } else {
        if (isEqual((*fontIt)->fontSize(), fontSize)) {
            if (!loadFontBFF(fontIt->get())) {
                excludeFontBFF(fontIt->get());
                return nullptr;
            }
            return *fontIt;
        }
        if (fontIt == fonts.begin()) {
            originFont = fonts.front().get();
        } else {
            auto fontItPrev = fontIt - 1;
            float distToPrev = fontSize - (*fontItPrev)->fontSize();
            float distToNext = (*fontIt)->fontSize() - fontSize;
            originFont = distToPrev < distToNext ? fontItPrev->get() : fontIt->get();
        }
    }
    
    if (!loadFontBFF(originFont)) {
        excludeFontBFF(originFont);
        return nullptr;
    }
    return std::make_shared<ScaledFont>(originFont, fontSize);
}

void FontStorage::excludeFontBFF(FontBFF* font) const
{
    auto familyIt = m_fontFamilies.find(font->familyName());
    if (familyIt == m_fontFamilies.end())
        return;
    auto& family = familyIt->second;
    auto fontIt = std::find_if(family.begin(), family.end(),
        [font](const auto& fontSPtr) { return fontSPtr.get() == font; });
    if (fontIt == family.end())
        return;
    family.erase(fontIt);
}

const FontStorage& fontStorage()
{
    return globalResources().fontStorage;
}

} }
