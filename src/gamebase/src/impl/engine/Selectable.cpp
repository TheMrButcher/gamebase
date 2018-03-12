/**
 * Copyright (c) 2018 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/engine/Selectable.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>

namespace gamebase { namespace impl {

void Selectable::registerSelectionState(PropertiesRegisterBuilder* builder)
{
    builder->registerPropertyWithSetter("selectionState", &m_selectionState,
        [this](SelectionState::Enum value) { setSelectionState(value); });
}

} }
