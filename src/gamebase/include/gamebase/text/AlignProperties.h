#pragma once

#include <gamebase/text/FontStorage.h>

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

}
