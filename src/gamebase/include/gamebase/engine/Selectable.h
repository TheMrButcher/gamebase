#pragma once

#include <gamebase/engine/ISelectable.h>

namespace gamebase {

class PropertiesRegisterBuilder;

class Selectable : public ISelectable {
public:
    Selectable()
        : m_selectionState(SelectionState::None)
        , m_associatedSelectable(nullptr)
    {}

    virtual SelectionState::Enum selectionState() const override
    {
        return m_selectionState;
    }

    virtual ISelectable* associatedSelectable() const override
    {
        return m_associatedSelectable;
    }

    virtual void setAssociatedSelectable(ISelectable* selectable) override
    {
        m_associatedSelectable = selectable;
    }

protected:
    void registerSelectionState(PropertiesRegisterBuilder* builder);

    SelectionState::Enum m_selectionState;
    ISelectable* m_associatedSelectable;
};

}
