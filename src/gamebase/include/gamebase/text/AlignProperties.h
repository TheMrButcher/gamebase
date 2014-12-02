#pragma once

#include <gamebase/text/IFont.h>

namespace gamebase {

struct HorAlign {
    enum Enum {
        Left,
        Center,
        Right
    };
};

struct VertAlign {
    enum Enum {
        Top,
        Center,
        Bottom
    };
};

struct AlignProperties {
    AlignProperties(const IFont* font)
        : font(font)
        , horAlign(HorAlign::Left)
        , vertAlign(VertAlign::Top)
    {}

    const IFont* font;
    HorAlign::Enum horAlign;
    VertAlign::Enum vertAlign;
};

}
