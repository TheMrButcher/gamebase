/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <list>

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

private:
    struct SoundBufferDesc {
        std::shared_ptr<sf::SoundBuffer> bufferPtr;
        std::string filePath;
    };
    typedef std::list<SoundBufferDesc> SoundBufferList;
    SoundBufferList::iterator shrinkAndPreload(const std::string& filePath);
    void preloadAllImpl(const std::string& dirPath);
    SoundBufferList::iterator preloadImpl(const std::string& filePath);
    void shrinkIfNeeded(size_t maxSize);

    size_t m_maxSize;
    SoundBufferList m_buffers;
    std::unordered_map<std::string, SoundBufferList::iterator> m_nameToBuffer;
};

} }
