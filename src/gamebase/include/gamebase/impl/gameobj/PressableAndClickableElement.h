#pragma once

#include <gamebase/impl/gameobj/ClickableElement.h>

namespace gamebase { namespace impl {

class GAMEBASE_API PressableAndClickableElement : public ClickableElement {
public:
    virtual void step() override
    {
        if ((isPressed() || m_isJustOutpressed) && m_callback)
            m_callback();
        ClickableElement::step();
    }
};

} }
