#pragma once

#include <gamebase/engine/SelectableElement.h>

namespace gamebase {

class PropertiesRegisterBuilder;

class GAMEBASE_API ClickableElement : public SelectableElement {
public:
    virtual void setCallback(const std::function<void()>& callback) override
    {
        m_callback = callback;
    }

    virtual void setTransitionCallback(
        const std::function<void(SelectionState::Enum)>& callback)
    {
        m_transitionCallback = callback;
    }

    virtual void setSelectionState(SelectionState::Enum state) override;

    virtual void registerObject(PropertiesRegisterBuilder*) override {}
    virtual void serialize(Serializer&) const override {}

private:
    std::function<void()> m_callback;
    std::function<void(SelectionState::Enum)> m_transitionCallback;
};

}
