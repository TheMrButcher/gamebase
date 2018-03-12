/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/skin/base/CheckBoxSkin.h>
#include <gamebase/impl/skin/tools/AnimatedObject.h>

namespace gamebase { namespace impl {

class GAMEBASE_API AnimatedCheckBoxSkin : public CheckBoxSkin, public AnimatedObject {
public:
    AnimatedCheckBoxSkin(
        const std::shared_ptr<IRelativeBox>& box,
        const std::shared_ptr<IRelativeGeometry>& geom = nullptr)
        : AnimatedObject(box, geom)
    {}

    void setCheckAnimation(const std::shared_ptr<IAnimation>& anim)
    {
        m_checkAnimation = anim;
    }
    
    void setUncheckAnimation(const std::shared_ptr<IAnimation>& anim)
    {
        m_uncheckAnimation = anim;
    }
    
    virtual std::shared_ptr<IRelativeGeometry> geometry() const override
    {
        return m_geom;
    }

    virtual void setChecked(bool status) override;

    virtual void loadResources() override;

    virtual void serialize(Serializer& s) const override;

private:
    std::shared_ptr<IAnimation> m_checkAnimation;
    std::shared_ptr<IAnimation> m_uncheckAnimation;
};

} }
