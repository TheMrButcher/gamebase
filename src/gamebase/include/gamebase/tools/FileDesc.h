/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <string>

namespace gamebase {

struct FileDesc {
    enum Type {
        None,
        Directory,
        File,
        Other
    };

    FileDesc(): type(None) {}
    FileDesc(
        const std::string& path,
        const std::string& fileName,
        const std::string& extension,
        Type type)
        : path(path)
        , fileName(fileName)
        , extension(extension)
        , type(type)
    {}

    std::string fullName() const
    {
        return extension.empty() ? fileName : fileName + '.' + extension;
    }

    std::string path;
    std::string fileName;
    std::string extension;
    Type type;
};

}
