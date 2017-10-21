/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

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
