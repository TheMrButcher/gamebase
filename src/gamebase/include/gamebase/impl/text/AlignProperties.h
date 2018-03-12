/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/text/FontDesc.h>
#include <gamebase/impl/relpos/AlignEnums.h>

namespace gamebase { namespace impl {

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
