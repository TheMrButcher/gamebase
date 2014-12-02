#include <stdafx.h>
#include <gamebase/utils/FileIO.h>
#include <gamebase/utils/Exception.h>
#include <fstream>
#include <sstream>

namespace gamebase {

std::string loadTextFile(const std::string& name)
{
    std::ifstream file(name);
    if (!file.good())
        THROW_EX() << "Can't open file: " << name;
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::vector<char> loadBinaryFile(const std::string& name)
{
    std::ifstream file(name, std::ios_base::binary);
    if (!file.good())
        THROW_EX() << "Can't open file: " << name;
    file.seekg(0, std::ios_base::end);
    size_t fileSize = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios_base::beg);
    
    std::vector<char> result(fileSize);
    file.read(&result.front(), fileSize);
    return result;
}

}
