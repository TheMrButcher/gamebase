#pragma once

#include <gamebase/engine/TextureRect.h>

namespace gamebase {

class GAMEBASE_API FilledRect : public TextureRect {
public:
    FilledRect(const IPositionable* position = nullptr)
        : TextureRect(position)
    {}

    virtual void loadResources() override;
};

}
