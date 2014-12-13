#pragma once

#include <gamebase/engine/IObject.h>

namespace gamebase {

struct SelectionState {
    enum Enum {
        None,
        MouseOn,
        Pressed,
        Selected
    };
};

class ISelectable : public virtual IObject {
public:
    virtual SelectionState::Enum selectionState() const = 0;

    virtual void setSelectionState(SelectionState::Enum state) = 0;
};

}
