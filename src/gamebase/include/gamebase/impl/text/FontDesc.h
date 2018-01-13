/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <string>
#include <memory>

namespace gamebase { namespace impl {

class IFont;

struct FontDesc {
    FontDesc() = default;
    FontDesc(const std::string& fontFamily, float size)
        : fontFamily(fontFamily)
        , size(size)
    {}

    std::shared_ptr<IFont> get() const;

    std::string fontFamily = "Roboto";
    float size = 20;

    bool bold = false;
    bool italic = false;
    bool underlined = false;
    bool lineThrough = false;
    float outlineWidth = 0.f;
};

} }
