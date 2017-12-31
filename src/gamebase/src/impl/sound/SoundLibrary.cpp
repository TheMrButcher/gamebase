/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/sound/SoundLibrary.h>
#include "src/impl/global/Config.h"
#include <gamebase/tools/Exception.h>
#include <gamebase/tools/FileIO.h>
#include <gamebase/text/StringUtils.h>
#include <SFML/Audio/SoundBuffer.hpp>
#include <iostream>

namespace gamebase { namespace impl {

namespace {
const size_t DEFAULT_MAX_BUFFERS_NUM = 256;

std::string makePath(const std::string& path)
{
    return config().soundsPath + path;
}
}

SoundLibrary::SoundLibrary()
    : m_maxSize(DEFAULT_MAX_BUFFERS_NUM)
{}

void SoundLibrary::preloadAll(const std::string& dirPath)
{
    preloadAllImpl(dirPath);
    shrinkIfNeeded(m_maxSize);
}

void SoundLibrary::preload(const std::string& filePath)
{
    shrinkAndPreload(filePath);
}

std::shared_ptr<sf::SoundBuffer> SoundLibrary::load(const std::string& filePath)
{
    return shrinkAndPreload(filePath)->bufferPtr;
}

size_t SoundLibrary::maxSize() const
{
    return m_maxSize;
}

void SoundLibrary::setMaxSize(size_t size)
{
    m_maxSize = size;
    shrinkIfNeeded(size);
}

bool SoundLibrary::has(const std::string& filePath) const
{
    return m_nameToBuffer.count(filePath) > 0;
}

SoundLibrary::SoundBufferList::iterator SoundLibrary::shrinkAndPreload(const std::string& filePath)
{
    auto it = m_nameToBuffer.find(filePath);
    if (it != m_nameToBuffer.end())
        return it->second;
    shrinkIfNeeded(m_maxSize - 1);
    return preloadImpl(filePath);
}

void SoundLibrary::preloadAllImpl(const std::string& dirPath)
{
    auto fullPath = makePath(dirPath);
    auto fileDescs = listFilesInDirectory(fullPath);
    auto prefix = addSlash(dirPath);
    for (const auto& desc : fileDescs) {
        if (desc.type == FileDesc::Directory) {
            auto pathToDir = prefix + desc.fullName();
            preloadAllImpl(pathToDir);
        }

        if (desc.type == FileDesc::File) {
            if (desc.extension == "ogg" || desc.extension == "wav" || desc.extension == "flac") {
                auto pathToFile = prefix + desc.fullName();
                if (!has(pathToFile))
                    preloadImpl(pathToFile);
            }
        }
    }
}

SoundLibrary::SoundBufferList::iterator SoundLibrary::preloadImpl(const std::string& filePath)
{
    auto buffer = std::make_shared<sf::SoundBuffer>();
    if (!buffer->loadFromFile(makePath(filePath)))
        THROW_EX() << "Error while loading sound: " << filePath;
    m_buffers.emplace_front(SoundBufferDesc{ buffer, filePath });
    auto it = m_buffers.begin();
    m_nameToBuffer[filePath] = it;
    std::cout << "Loaded sound: " << filePath << std::endl;
    return it;
}

void SoundLibrary::shrinkIfNeeded(size_t maxSize)
{
    while (m_buffers.size() > maxSize) {
        const auto& last = m_buffers.back();
        m_nameToBuffer.erase(last.filePath);
        m_buffers.pop_back();
    }
}

} }
