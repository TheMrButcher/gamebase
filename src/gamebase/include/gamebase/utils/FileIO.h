#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/utils/FileDesc.h>
#include <string>
#include <vector>

namespace gamebase {

std::string GAMEBASE_API loadTextFile(const std::string& name);

std::vector<char> GAMEBASE_API loadBinaryFile(const std::string& name);

bool GAMEBASE_API fileExists(const std::string& pathStr);

FileDesc GAMEBASE_API fileInfo(const std::string& pathStr);

std::vector<FileDesc> GAMEBASE_API listFilesInDirectory(const std::string& pathStr);

}
