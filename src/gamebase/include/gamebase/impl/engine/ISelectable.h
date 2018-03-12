/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/IObject.h>

namespace gamebase { namespace impl {

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

} }
