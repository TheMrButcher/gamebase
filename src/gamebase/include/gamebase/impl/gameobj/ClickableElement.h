/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/gameobj/SelectableElement.h>
#include <gamebase/impl/tools/Timer.h>

namespace gamebase { namespace impl {

class PropertiesRegisterBuilder;

class GAMEBASE_API ClickableElement : public SelectableElement {
public:
    static const Time DEFAULT_CLICK_TIME = 500;

    ClickableElement();

    Time clickTime() const { return m_clickTime; }
    void setClickTime(Time t)
    {
        if (t <= 0)
            t = DEFAULT_CLICK_TIME;
        m_clickTime = t;
    }

    virtual void setCallback(const std::function<void()>& callback) override
    {
        m_callback = callback;
    }

    virtual void setTransitionCallback(
        const std::function<void(SelectionState::Enum)>& callback) override
    {
        m_transitionCallback = callback;
    }

    virtual void step() override
    {
        if (m_isClicked && m_callback)
            m_callback();
        m_isClicked = false;
        m_isJustOutpressed = false;
    }

    virtual bool isMouseOn() const override
    {
        return m_selectionState == SelectionState::MouseOn
            || m_selectionState == SelectionState::Pressed;
    }

    virtual bool isPressed() const override
    {
        return m_selectionState == SelectionState::Pressed;
    }

    virtual bool isClicked() const override { return m_isClicked; }

    virtual void setSelectionState(SelectionState::Enum state) override;

    virtual void registerObject(PropertiesRegisterBuilder*) override {}
    virtual void serialize(Serializer&) const override;

protected:
    bool m_isJustOutpressed;
    std::function<void()> m_callback;
    Time m_clickTime;

private:
    std::function<void(SelectionState::Enum)> m_transitionCallback;
    Timer m_timer;
    bool m_isClicked;
};

} }
