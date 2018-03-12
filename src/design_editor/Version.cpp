/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "Version.h"
#include <fstream>
#include <sstream>

namespace gamebase { namespace editor {

namespace {
std::string readVersion()
{
    std::ifstream file;
    file.open("VERSION.txt");
    if (!file.is_open()) {
        file.open("..\\..\\..\\VERSION.txt");
        if (!file.is_open())
            return "0.0.0";
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}
}

std::string version()
{
    static const std::string VERSION_STR = readVersion();
    return VERSION_STR;
}

} }
