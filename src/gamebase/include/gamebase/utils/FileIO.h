#pragma once

#include <gamebase/GameBaseAPI.h>
#include <string>
#include <vector>

namespace gamebase {

std::string GAMEBASE_API loadTextFile(const std::string& name);

std::vector<char> GAMEBASE_API loadBinaryFile(const std::string& name);

}
