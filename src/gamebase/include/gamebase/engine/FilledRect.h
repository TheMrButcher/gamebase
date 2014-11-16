#pragma once

#include <gamebase/engine/TextureRect.h>

namespace gamebase {

class FilledRect : public TextureRect {
public:
    FilledRect(const IPositionable* position)
        : TextureRect(position)
    {}

    virtual void loadResources() override;
};

}
