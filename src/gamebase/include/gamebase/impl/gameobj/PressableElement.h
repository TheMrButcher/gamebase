/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/gameobj/SelectableElement.h>
#include <gamebase/tools/Delayed.h>

namespace gamebase { namespace impl {

class PropertiesRegisterBuilder;

class GAMEBASE_API PressableElement : public SelectableElement {
public:
    PressableElement()
        : m_isJustOutpressed(false)
    {}

    virtual void setCallback(const std::function<void()>& callback) override
    {
        m_callback = callback;
    }

    virtual void setTransitionCallback(
        const std::function<void(SelectionState::Enum)>& callback)
    {
        m_transitionCallback = callback;
    }

    virtual void step() override
    {
        if ((isPressed() || m_isJustOutpressed) && m_callback)
            m_callback();
        m_isJustOutpressed = false;
    }

    virtual bool isMouseOn() const
    {
        return m_selectionState == SelectionState::MouseOn
            || m_selectionState == SelectionState::Pressed;
    }

    virtual bool isPressed() const
    {
        return m_selectionState == SelectionState::Pressed;
    }

    virtual bool isClicked() const { return false; }

    virtual void setSelectionState(SelectionState::Enum state) override;

    virtual void registerObject(PropertiesRegisterBuilder*) override {}
    virtual void serialize(Serializer&) const override {}

private:
    std::function<void()> m_callback;
    std::function<void(SelectionState::Enum)> m_transitionCallback;
    bool m_isJustOutpressed;
};

} }
