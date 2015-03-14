#include <stdafx.h>
#include <gamebase/engine/Selectable.h>
#include <gamebase/engine/PropertiesRegisterBuilder.h>

namespace gamebase {

void Selectable::registerSelectionState(PropertiesRegisterBuilder* builder)
{
    builder->registerProperty("selectionState", &m_selectionState,
        std::bind(&Selectable::setSelectionState, this, std::placeholders::_1));
}

}
