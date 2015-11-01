#pragma once

#include <gamebase/engine/IObject.h>

namespace gamebase {

struct SelectionState {
    enum Enum {
        None,
        Disabled,
        MouseOn,
        Pressed,
        Selected
    };
};

class ISelectable : public virtual IObject {
public:
    virtual SelectionState::Enum selectionState() const = 0;

    virtual void setSelectionState(SelectionState::Enum state) = 0;

    virtual ISelectable* associatedSelectable() const = 0;

    virtual void setAssociatedSelectable(ISelectable* selectable) = 0;

    virtual void disable()
    {
        setSelectionState(SelectionState::Disabled);
    }

    virtual void enable()
    {
        if (selectionState() == SelectionState::Disabled)
            setSelectionState(SelectionState::None);
    }
};

}
