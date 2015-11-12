#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/engine/ButtonSkin.h>
#include <gamebase/engine/AnimatedObject.h>

namespace gamebase {

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

}
