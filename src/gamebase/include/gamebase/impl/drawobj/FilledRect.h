/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

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
