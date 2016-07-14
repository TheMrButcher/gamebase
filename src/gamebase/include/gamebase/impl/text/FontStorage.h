/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/text/IFont.h>
#include <unordered_map>

namespace gamebase { namespace impl {

class GAMEBASE_API FontStorage {
public:
    FontStorage() {}

    void load(const std::string& fontsPath);
    std::shared_ptr<IFont> getFont(float fontSize) const;
    std::shared_ptr<IFont> getFont(const std::string& familyName, float fontSize) const;

private:
    void loadFont(const std::string& fname);

    std::shared_ptr<IFont> findFont(
        const std::vector<std::shared_ptr<IFont>>& fonts,
        float fontSize) const;

    std::unordered_map<std::string, std::vector<std::shared_ptr<IFont>>> m_fontFamilies;
    std::string m_defaultFamilyName;
};

GAMEBASE_API const FontStorage& fontStorage();

} }
