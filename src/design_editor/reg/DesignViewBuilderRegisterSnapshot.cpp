/**
 * Copyright (c) 2016 Slavnejshev Filipp
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
    addAs<impl::FixedOffset>("AligningOffset", &serializeFixedOffsetAsAligning);
    add<impl::AligningOffset>(&deserializeOffset);
        
    addAs<impl::RelativeBox>("ComplexBox", &serializeRelativeBoxAsComplex);
    addAs<impl::OffsettedBox>("ComplexBox", &serializeOffsettedBoxAsComplex);
    addAs<impl::SquareBox>("ComplexBox", &serializeSquareBoxAsComplex);
    addAs<impl::FixedBox>("ComplexBox", &serializeFixedBoxAsComplex);
    addAs<impl::PixelBox>("ComplexBox", &serializePixelBoxAsComplex);
    add<impl::ComplexBox>(&deserializeBox);

    addAs<impl::InactiveObjectConstruct>("ObjectConstruct", &serializeInactiveObjectConstruct);
    addAs<impl::AnimatedObjectConstruct>("ObjectConstruct", &serializeAnimatedObjectConstruct);
    add<impl::ObjectConstruct>(&deserializeObjectConstruct, &serializeObjectConstruct);

    addAs<impl::PressableElement>("CommonSelectableElement", &serializePressableElement);
    addAs<impl::ClickableElement>("CommonSelectableElement", &serializeClickableElement);
    addAs<impl::PressableAndClickableElement>("CommonSelectableElement", &serializePressableAndClickableElement);

    add<impl::LinearLayout>(&deserializeLinearLayout);
}

} }
