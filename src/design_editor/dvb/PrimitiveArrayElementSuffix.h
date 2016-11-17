/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/serial/constants.h>
#include <vector>

namespace gamebase { namespace editor {

class PrimitiveArrayElementSuffix {
public:
    PrimitiveArrayElementSuffix();

    const std::string& get(impl::SerializationTag::Type tag, size_t index) const
    {
        return m_suffixes.at(tag).at(index);
    }

private:
    std::vector<std::vector<std::string>> m_suffixes;
};

const PrimitiveArrayElementSuffix PRIMITIVE_ARRAY_ELEMENT_SUFFIX;

} }
