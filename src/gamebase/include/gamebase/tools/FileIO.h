/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/tools/FileDesc.h>
#include <string>
#include <vector>

namespace gamebase {

GAMEBASE_API std::string loadTextFile(const std::string& name);

GAMEBASE_API std::vector<char> loadBinaryFile(const std::string& name);

GAMEBASE_API bool fileExists(const std::string& pathStr);

GAMEBASE_API FileDesc fileInfo(const std::string& pathStr);

GAMEBASE_API std::vector<FileDesc> listFilesInDirectory(const std::string& pathStr);

GAMEBASE_API std::string absolutePath(const std::string& pathStr);

GAMEBASE_API std::string normalizePath(const std::string& pathStr);

GAMEBASE_API std::string relativePath(const std::string& parentPathStr, const std::string& childPathStr);

GAMEBASE_API void renameFile(const std::string& oldPathStr, const std::string& newPathStr);

GAMEBASE_API void removeFile(const std::string& pathStr);

GAMEBASE_API bool isAbsolutePath(const std::string& pathStr);

GAMEBASE_API void createDir(const std::string& pathStr);

GAMEBASE_API std::string makePathStr(
    const std::string& path,
    const std::string& fileName,
    const std::string& extension);

GAMEBASE_API std::string fileName(const std::string& pathStr);

GAMEBASE_API std::string pathToDir(const std::string& pathStr);

}
