/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "DesignViewBuilderRegisterSnapshot.h"
#include <reg/overriders/RelativeOffsets.h>
#include <reg/overriders/RelativeBoxes.h>
#include <reg/overriders/GameObj.h>
#include <reg/overriders/SelectableElements.h>
#include <reg/overriders/LinearLayout.h>

namespace gamebase { namespace editor {

DesignViewBuilderRegisterSnapshot::DesignViewBuilderRegisterSnapshot()
    : RegisterSnapshot(RegisterSwitcher::DVBReg)
{
	add<impl::FixedOffset>(&deserializeFixedOffset);
	add<impl::AligningOffset>(&deserializeAligningOffset);
        
    add<impl::RelativeBox>(&deserializeRelativeBox);
    add<impl::OffsettedBox>(&deserializeOffsettedBox);
    add<impl::SquareBox>(&deserializeSquareBox);
    add<impl::FixedBox>(&deserializeFixedBox);
    add<impl::PixelBox>(&deserializePixelBox);
	add<impl::ComplexBox>(&deserializeComplexBox);

    addAs<impl::InactiveObjectConstruct>("ObjectConstruct", &serializeInactiveObjectConstruct);
    addAs<impl::AnimatedObjectConstruct>("ObjectConstruct", &serializeAnimatedObjectConstruct);
    add<impl::ObjectConstruct>(&deserializeObjectConstruct, &serializeObjectConstruct);

    addAs<impl::PressableElement>("CommonSelectableElement", &serializePressableElement);
    addAs<impl::ClickableElement>("CommonSelectableElement", &serializeClickableElement);
    addAs<impl::PressableAndClickableElement>("CommonSelectableElement", &serializePressableAndClickableElement);

    add<impl::LinearLayout>(&deserializeLinearLayout);
}

} }
