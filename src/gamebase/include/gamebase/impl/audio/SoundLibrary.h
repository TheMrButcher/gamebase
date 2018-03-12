/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/tools/Cache.h>
#include <string>

namespace sf {
class SoundBuffer;
}

namespace gamebase { namespace impl {

class SoundLibrary {
public:
    SoundLibrary();

    void preloadAll(const std::string& dirPath);
    void preload(const std::string& filePath);
    std::shared_ptr<sf::SoundBuffer> load(const std::string& filePath);
    size_t maxSize() const;
    void setMaxSize(size_t size);
    bool has(const std::string& filePath) const;
    void clear();

private:
    std::shared_ptr<sf::SoundBuffer> shrinkAndPreload(const std::string& filePath);
    void preloadAllImpl(const std::string& dirPath);
    std::shared_ptr<sf::SoundBuffer> preloadImpl(const std::string& filePath);

    Cache<std::string, sf::SoundBuffer> m_cache;
};

} }
