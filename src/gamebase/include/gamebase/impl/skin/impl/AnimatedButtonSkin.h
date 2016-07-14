/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/skin/base/ButtonSkin.h>
#include <gamebase/impl/skin/tools/AnimatedObject.h>

namespace gamebase { namespace impl {

class GAMEBASE_API AnimatedButtonSkin : public ButtonSkin, public AnimatedObject {
public:
    AnimatedButtonSkin(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeGeometry>& geom = nullptr)
        : AnimatedObject(box, geom)
    {}
    
    virtual std::shared_ptr<IRelativeGeometry> geometry() const override
    {
        return m_geom;
    }

    void setFixedBox(float width, float height);
};

} }
