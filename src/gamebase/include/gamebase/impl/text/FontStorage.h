/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/text/IFont.h>
#include <gamebase/impl/text/FontDescSFML.h>
#include <gamebase/impl/tools/Cache.h>
#include <boost/optional.hpp>
#include <unordered_map>

namespace gamebase {
struct FileDesc;
}

namespace sf {
class Font;
}

namespace gamebase { namespace impl {

class FontBFF;
struct FontDescSFML;

class GAMEBASE_API FontStorage {
public:
    FontStorage();
    ~FontStorage();

    struct FontFamily {
        std::string name;
        enum Type {
            SFML,
            BFF
        };
        Type type;
    };

    const std::vector<FontFamily>& fontList() const { return m_fontList; }
    const std::string& defaultFamilyNameBFF() const { return m_defaultFamilyNameBFF; }
    void load(const std::string& fontsPath);
    void prepare();
    boost::optional<FontFamily::Type> typeOf(const std::string& familyName) const;
    std::shared_ptr<IFont> getFont(float fontSize) const;
    std::shared_ptr<IFont> getFont(const std::string& familyName, float fontSize) const;
    std::shared_ptr<IFont> getFont(
        const std::string& familyName, float fontSize,
        bool bold, bool italic, float outlineWidth) const;
    void clear();

private:
    void extractFontsBFF(const std::vector<FileDesc>& files);
    void extractFontsSFML(const std::vector<FileDesc>& files);
    std::shared_ptr<IFont> findFontBFF(
        const std::vector<std::shared_ptr<FontBFF>>& fonts,
        float fontSize) const;
    void excludeFontBFF(FontBFF* font) const;
    std::shared_ptr<IFont> findFontSFML(
        const std::string& familyName, std::vector<FontDescSFML>& fonts,
        float fontSize, bool bold, bool italic, float outlineWidth) const;

    std::vector<FontFamily> m_fontList;

    mutable std::unordered_map<std::string, std::vector<std::shared_ptr<FontBFF>>> m_fontFamiliesBFF;
    mutable std::string m_defaultFamilyNameBFF;

    mutable std::unordered_map<std::string, std::vector<FontDescSFML>> m_fontFamiliesSFML;
    mutable Cache<std::string, sf::Font> m_fontsSFML;
    mutable std::string m_defaultFamilyNameSFML;
};

GAMEBASE_API const FontStorage& fontStorage();

} }
