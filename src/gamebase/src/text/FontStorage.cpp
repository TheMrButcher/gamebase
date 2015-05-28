#include <stdafx.h>
#include <gamebase/text/FontStorage.h>
#include "Font.h"
#include "ScaledFont.h"
#include "src/core/GlobalResources.h"
#include <gamebase/utils/FileIO.h>
#include <gamebase/math/Math.h>
#include <iostream>

namespace gamebase {
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
}

void FontStorage::load(const std::string& fontsPath)
{
    std::cout << "Loading fonts..." << std::endl;
    auto fontDirFilesList = listFilesInDirectory(fontsPath);
    std::vector<std::string> fontFileNames;
    for (auto it = fontDirFilesList.begin(); it != fontDirFilesList.end(); ++it) {
        if (it->type == FileDesc::File && it->extension == "bff")
            fontFileNames.push_back(it->path);
    }

    for (auto it = fontFileNames.begin(); it != fontFileNames.end(); ++it)
        loadFont(*it);

    for (auto it = m_fontFamilies.begin(); it != m_fontFamilies.end(); ++it) {
        auto& fonts = it->second;
        std::sort(fonts.begin(), fonts.end(), compareFonts);
    }

    if (m_fontFamilies.empty()) {
        std::cerr << "Warning! No fonts are loaded" << std::endl;
        return;
    }

    auto it = m_fontFamilies.find(DEFAULT_FONT);
    m_defaultFamilyName = it == m_fontFamilies.end()
        ? m_fontFamilies.begin()->first
        : DEFAULT_FONT;

    std::cout << "Done loading fonts" << std::endl;
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
    auto& fonts = it == m_fontFamilies.end()
        ? m_fontFamilies.at(m_defaultFamilyName)
        : it->second;
    return findFont(fonts, fontSize);
}

void FontStorage::loadFont(const std::string& fname)
{
    try {
        std::string metaDataFileName =
            fname.substr(0, fname.find_last_of(".")) + ".json";
        auto font = std::make_shared<Font>(fname, metaDataFileName);
        font->load();
        std::cout << "Loaded font: \"" << font->familyName() << "\", height: " << font->fontSize() << std::endl;
        m_fontFamilies[font->familyName()].push_back(font);
    } catch (Exception& ex) {
        std::cerr << "Can't load font from file: " << fname << ". Reason: " << ex.what() << std::endl;
        return;
    }
}

std::shared_ptr<IFont> FontStorage::findFont(const std::vector<std::shared_ptr<IFont>>& fonts, float fontSize) const
{
    auto fontIt = std::lower_bound(fonts.begin(), fonts.end(), fontSize, compareSizes);
    const IFont* originFont = nullptr;
    if (fontIt == fonts.end()) {
        originFont = fonts.back().get();
    } else {
        if (isEqual((*fontIt)->fontSize(), fontSize)) {
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
            
    return std::make_shared<ScaledFont>(originFont, fontSize);
}

const FontStorage& fontStorage()
{
    return globalResources().fontStorage;
}

}
