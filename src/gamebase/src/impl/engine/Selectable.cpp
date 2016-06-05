#include <stdafx.h>
#include <gamebase/impl/engine/Selectable.h>
#include <gamebase/impl/reg/PropertiesRegisterBuilder.h>

namespace gamebase { namespace impl {

void Selectable::registerSelectionState(PropertiesRegisterBuilder* builder)
{
    builder->registerProperty("selectionState", &m_selectionState,
        std::bind(&Selectable::setSelectionState, this, std::placeholders::_1));
}

} }
