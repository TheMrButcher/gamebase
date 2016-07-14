/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/text/FontStorage.h>
#include <gamebase/impl/relpos/AlignEnums.h>

namespace gamebase { namespace impl {

struct FontDesc {
    FontDesc()
        : fontFamily("Arial")
        , size(20)
    {}

    FontDesc(const std::string& fontFamily, float size)
        : fontFamily(fontFamily)
        , size(size)
    {}

    std::shared_ptr<IFont> get() const
    {
        return fontStorage().getFont(fontFamily, size);
    }

    std::string fontFamily;
    float size;
};

struct AlignProperties {
    AlignProperties()
        : horAlign(HorAlign::Left)
        , vertAlign(VertAlign::Top)
        , enableStacking(true)
    {}

    FontDesc font;
    HorAlign::Enum horAlign;
    VertAlign::Enum vertAlign;
    bool enableStacking;
};

} }
