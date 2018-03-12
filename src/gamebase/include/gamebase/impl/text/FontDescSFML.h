/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <boost/optional.hpp>
#include <string>

namespace gamebase {
struct FileDesc;
}

namespace gamebase { namespace impl {

struct FontDescSFML {
    std::string family;
    std::string path;
    bool bold;
    bool italic;

    static FontDescSFML fromFile(const FileDesc& file);
};

} }