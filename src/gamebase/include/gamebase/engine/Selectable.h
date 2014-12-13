#pragma once

#include <gamebase/engine/ISelectable.h>

namespace gamebase {

class Selectable : public ISelectable {
public:
    Selectable() : m_selectionState(SelectionState::None) {}

    virtual SelectionState::Enum selectionState() const override
    {
        return m_selectionState;
    }

protected:
    SelectionState::Enum m_selectionState;
};

}
