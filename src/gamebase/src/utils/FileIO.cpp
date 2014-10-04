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

}
