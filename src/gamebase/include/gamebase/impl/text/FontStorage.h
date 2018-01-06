/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/text/IFont.h>
#include <unordered_map>

namespace gamebase {
struct FileDesc;
}

namespace gamebase { namespace impl {

class FontBFF;

class GAMEBASE_API FontStorage {
public:
    FontStorage();

	const std::vector<std::string>& fontNames() const { return m_fontNames; }
	const std::string& defaultFamilyName() const { return m_defaultFamilyName; }
    void load(const std::string& fontsPath);
    void prepare();
    std::shared_ptr<IFont> getFont(float fontSize) const;
    std::shared_ptr<IFont> getFont(const std::string& familyName, float fontSize) const;

private:
    void extractFontsBFF(const std::vector<FileDesc>& files);
    std::shared_ptr<IFont> findFontBFF(
        const std::vector<std::shared_ptr<FontBFF>>& fonts,
        float fontSize) const;
    void excludeFontBFF(FontBFF* font) const;

	std::vector<std::string> m_fontNames;
    mutable std::unordered_map<std::string, std::vector<std::shared_ptr<FontBFF>>> m_fontFamilies;
    mutable std::string m_defaultFamilyName;
};

GAMEBASE_API const FontStorage& fontStorage();

} }
