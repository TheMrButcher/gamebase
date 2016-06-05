#pragma once

#include <gamebase/impl/drawobj/TextureRect.h>

namespace gamebase { namespace impl {

class GAMEBASE_API FilledRect : public TextureRect {
public:
    FilledRect(const IPositionable* position = nullptr)
        : TextureRect(position)
    {}

    virtual void loadResources() override;
};

} }
