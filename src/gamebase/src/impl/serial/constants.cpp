/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/serial/constants.h>
#include <gamebase/tools/Exception.h>

namespace gamebase { namespace impl {

const char* toString(SerializationVersion version)
{
    switch (version) {
    case SerializationVersion::VER3: return "VER3";
    default: THROW_EX() << "Unknown serialization version: " << static_cast<int>(version);
    }
}

} }
