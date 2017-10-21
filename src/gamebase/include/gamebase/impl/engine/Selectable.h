/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/impl/engine/ISelectable.h>

namespace gamebase { namespace impl {

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

} }
