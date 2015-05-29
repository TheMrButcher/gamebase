#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/CheckBoxSkin.h>
#include <gamebase/engine/AnimatedObject.h>

namespace gamebase {

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

}
