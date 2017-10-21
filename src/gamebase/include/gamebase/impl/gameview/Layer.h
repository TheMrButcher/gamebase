/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/gameview/ImmobileLayer.h>

namespace gamebase { namespace impl {

class GAMEBASE_API Layer : public ImmobileLayer {
public:
    Layer();
    
    virtual void drawAt(const Transform2& position) const override;

private:
    virtual const std::vector<Drawable*>& drawablesInView() const override;
};

} }
