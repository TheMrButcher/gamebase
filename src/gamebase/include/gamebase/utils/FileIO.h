#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/utils/FileDesc.h>
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

GAMEBASE_API void renameFile(const std::string& oldPathStr, const std::string& newPathStr);

GAMEBASE_API void removeFile(const std::string& pathStr);

GAMEBASE_API bool isAbsolutePath(const std::string& pathStr);

GAMEBASE_API void createDir(const std::string& pathStr);

GAMEBASE_API std::string makePathStr(
    const std::string& path,
    const std::string& fileName,
    const std::string& extension);

}
