/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/text/FontStorage.h>
#include "FontBFF.h"
#include "ScaledFont.h"
#include "FontSFML.h"
#include "src/impl/global/GlobalResources.h"
#include <gamebase/tools/FileIO.h>
#include <gamebase/math/Math.h>
#include <SFML/Graphics/Font.hpp>
#include <iostream>

namespace gamebase { namespace impl {
namespace {
const std::string DEFAULT_FONT_BFF("Arial");
const size_t MAX_CACHE_SIZE_SFML = 32;
const std::string DEFAULT_FONT_SFML("Roboto");

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
        std::cout << "Loaded BFF font: " << font->familyName() << ", height: " << font->fontSize() << std::endl;
        return true;
    }
    catch (std::exception& ex) {
        std::cerr << "Can't load BFF font from file: " << font->fileName()
            << ". Reason: " << ex.what() << std::endl;
        return false;
    }
}
}

FontStorage::FontStorage()
    : m_defaultFamilyNameBFF(DEFAULT_FONT_BFF)
    , m_fontsSFML(MAX_CACHE_SIZE_SFML)
    , m_defaultFamilyNameSFML(DEFAULT_FONT_SFML)
{}

FontStorage::~FontStorage() {}

void FontStorage::load(const std::string& fontsPath)
{
    auto files = listFilesInDirectory(fontsPath);
    extractFontsBFF(files);
    extractFontsSFML(files);
    for (const auto& file : files) {
        if (file.type == FileDesc::Directory)
            load(file.path);
    }
}

void FontStorage::prepare()
{
    m_fontList.clear();
    m_fontList.reserve(m_fontFamiliesBFF.size() + m_fontFamiliesSFML.size());
    for (auto& nameAndFamily : m_fontFamiliesBFF) {
        std::sort(nameAndFamily.second.begin(), nameAndFamily.second.end(), compareFonts);
        m_fontList.push_back(FontFamily{ nameAndFamily.first, FontFamily::BFF });
    }
    for (auto& nameAndFamily : m_fontFamiliesSFML)
        m_fontList.push_back(FontFamily{ nameAndFamily.first, FontFamily::SFML });
    std::sort(m_fontList.begin(), m_fontList.end(), [](const auto& font1, const auto& font2)
    {
        if (font1.type != font2.type)
            return font1.type < font2.type;
        return font1.name < font2.name;
    });

    if (m_fontFamiliesBFF.empty() && m_fontFamiliesSFML.empty()) {
        std::cerr << "Warning! No fonts are detected" << std::endl;
        return;
    }

    {
        auto it = m_fontFamiliesBFF.find(DEFAULT_FONT_BFF);
        m_defaultFamilyNameBFF = (it == m_fontFamiliesBFF.end() && !m_fontFamiliesBFF.empty())
            ? m_defaultFamilyNameBFF = m_fontFamiliesBFF.begin()->first
            : DEFAULT_FONT_BFF;
    }
    
    {
        auto it = m_fontFamiliesSFML.find(DEFAULT_FONT_SFML);
        m_defaultFamilyNameSFML = (it == m_fontFamiliesSFML.end() && !m_fontFamiliesSFML.empty())
            ? m_fontFamiliesSFML.begin()->first
            : DEFAULT_FONT_SFML;
    }
}

boost::optional<FontStorage::FontFamily::Type> FontStorage::typeOf(const std::string & familyName) const
{
    if (m_fontFamiliesSFML.count(familyName) > 0)
        return FontStorage::FontFamily::SFML;
    if (m_fontFamiliesBFF.count(familyName) > 0)
        return FontStorage::FontFamily::BFF;
    return boost::none;
}

std::shared_ptr<IFont> FontStorage::getFont(float fontSize) const
{
    return getFont(m_defaultFamilyNameBFF, fontSize);
}

std::shared_ptr<IFont> FontStorage::getFont(const std::string& familyName, float fontSize) const
{
    if (m_fontFamiliesBFF.empty())
        THROW_EX() << "Can't get any BFF font. FontStorage is empty";
    if (fontSize <= 0)
        THROW_EX() << "Wrong size of font: " << fontSize;
    auto it = m_fontFamiliesBFF.find(familyName);
    if (it == m_fontFamiliesBFF.end())
        it = m_fontFamiliesBFF.find(m_defaultFamilyNameBFF);
    if (it == m_fontFamiliesBFF.end()) {
        std::cerr << "Can't find nor " << familyName << " BFF font, neither default "
            << m_defaultFamilyNameBFF << " BFF font" << std::endl;
        m_defaultFamilyNameBFF = m_fontFamiliesBFF.begin()->first;
        std::cerr << m_defaultFamilyNameBFF << " is now default BFF font" << std::endl;
        it = m_fontFamiliesBFF.begin();
    }
    auto& fonts = it->second;
    auto font = findFontBFF(fonts, fontSize);
    if (!font) {
        if (fonts.empty())
            m_fontFamiliesBFF.erase(it);
        return getFont(familyName, fontSize);
    }
    return font;
}

std::shared_ptr<IFont> FontStorage::getFont(
    const std::string & familyName, float fontSize, bool bold, bool italic, float outlineWidth) const
{
    if (m_fontFamiliesSFML.empty()) {
        std::cerr << "Requesting normal font: " << familyName << ", but storage is empty. "
            << "Trying to find BFF font" << std::endl;
        return getFont(familyName, fontSize);
    }
    if (fontSize <= 0)
        THROW_EX() << "Wrong size of font: " << fontSize;
    auto it = m_fontFamiliesSFML.find(familyName);
    if (it == m_fontFamiliesSFML.end())
        it = m_fontFamiliesSFML.find(m_defaultFamilyNameSFML);
    if (it == m_fontFamiliesSFML.end()) {
        std::cerr << "Can't find nor " << familyName << " font, neither default "
            << m_defaultFamilyNameSFML << " font" << std::endl;
        m_defaultFamilyNameSFML = m_fontFamiliesSFML.begin()->first;
        std::cerr << m_defaultFamilyNameSFML << " is now default font" << std::endl;
        it = m_fontFamiliesSFML.begin();
    }
    auto& fonts = it->second;
    auto font = findFontSFML(it->first, fonts, fontSize, bold, italic, outlineWidth);
    if (!font) {
        if (fonts.empty())
            m_fontFamiliesSFML.erase(it);
        return getFont(familyName, fontSize, bold, italic, outlineWidth);
    }
    return font;
}

void FontStorage::clear()
{
    m_fontFamiliesBFF.clear();
    m_fontFamiliesSFML.clear();
    m_fontList.clear();
    m_fontsSFML.clear();
    m_defaultFamilyNameBFF = DEFAULT_FONT_BFF;
    m_defaultFamilyNameSFML = DEFAULT_FONT_SFML;
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
        m_fontFamiliesBFF[font->familyName()].push_back(font);
    }
}

void FontStorage::extractFontsSFML(const std::vector<FileDesc>& files)
{
    for (const auto& file : files) {
        if (file.type == FileDesc::File
                && (file.extension != "bff" && file.extension != "json" && file.extension != "txt")) {
            auto desc = FontDescSFML::fromFile(file);
            m_fontFamiliesSFML[desc.family].push_back(desc);
        }
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
    auto familyIt = m_fontFamiliesBFF.find(font->familyName());
    if (familyIt == m_fontFamiliesBFF.end())
        return;
    auto& family = familyIt->second;
    auto fontIt = std::find_if(family.begin(), family.end(),
        [font](const auto& fontSPtr) { return fontSPtr.get() == font; });
    if (fontIt == family.end())
        return;
    family.erase(fontIt);
}

std::shared_ptr<IFont> FontStorage::findFontSFML(
    const std::string& familyName, std::vector<FontDescSFML>& fonts,
    float fontSize, bool bold, bool italic, float outlineWidth) const
{
    bool makeBold = false;
    bool makeItalic = false;
    auto it = std::find_if(fonts.begin(), fonts.end(), [bold, italic](const FontDescSFML& font)
    {
        return font.bold == bold && font.italic == italic;
    });
    if (it == fonts.end()) {
        if (bold) {
            it = std::find_if(fonts.begin(), fonts.end(), [italic](const FontDescSFML& font)
            {
                return font.bold == false && font.italic == italic;
            });
            if (it != fonts.end())
                makeBold = true;
        }
    }
    if (it == fonts.end()) {
        it = std::find_if(fonts.begin(), fonts.end(), [](const FontDescSFML& font)
        {
            return font.bold == false && font.italic == false;
        });
        if (it != fonts.end()) {
            makeBold = bold;
            makeItalic = italic;
        }
    }
    if (it == fonts.end()) {
        it = fonts.begin();
        if (!it->bold && bold)
            makeBold = true;
        if (!it->italic && italic)
            makeItalic = true;
    }

    auto fontSFML = m_fontsSFML.get(it->path);
    if (!fontSFML) {
        fontSFML = std::make_shared<sf::Font>();
        if (!fontSFML->loadFromFile(it->path)) {
            std::cerr << "Can't load font from file: " << it->path << std::endl;
            fonts.erase(it);
            return nullptr;
        } else {
            std::cout << "Loaded font of family: " << familyName << ", path: " << it->path << std::endl;
        }
        m_fontsSFML.insert(it->path, fontSFML);
    }
    return std::make_shared<FontSFML>(
        fontSFML, familyName, uround(fontSize), makeBold, makeItalic, outlineWidth);
}

const FontStorage& fontStorage()
{
    return globalResources().fontStorage;
}

} }
