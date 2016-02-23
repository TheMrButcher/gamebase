#pragma once

#include <gamebase/engine/ClickableElement.h>

namespace gamebase {

class GAMEBASE_API PressableAndClickableElement : public ClickableElement {
public:
    virtual void step() override
    {
        if ((isPressed() || m_isJustOutpressed) && m_callback)
            m_callback();
        ClickableElement::step();
    }
};

}
