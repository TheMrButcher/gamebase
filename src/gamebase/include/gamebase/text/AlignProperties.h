#pragma once

#include <gamebase/text/FontStorage.h>
#include <gamebase/engine/AlignEnums.h>

namespace gamebase {

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
