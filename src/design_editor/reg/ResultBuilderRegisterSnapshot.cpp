/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "ResultBuilderRegisterSnapshot.h"
#include <reg/overriders/RelativeOffsets.h>
#include <reg/overriders/RelativeBoxes.h>
#include <reg/overriders/GameObj.h>
#include <reg/overriders/SelectableElements.h>

namespace gamebase { namespace editor {

ResultBuilderRegisterSnapshot::ResultBuilderRegisterSnapshot()
    : RegisterSnapshot(RegisterSwitcher::ResultReg)
{
    add<impl::FixedOffset>();
    add<impl::AligningOffset>(&deserializeOffset);

    add<impl::RelativeBox>();
    add<impl::OffsettedBox>();
    add<impl::SquareBox>();
    add<impl::FixedBox>();
    add<impl::PixelBox>();
    add<impl::ComplexBox>(&deserializeBox);

    add<impl::InactiveObjectConstruct>();
    add<impl::AnimatedObjectConstruct>();
    add<impl::ObjectConstruct>(&deserializeObjectConstruct);

    add<impl::PressableElement>();
    add<impl::ClickableElement>();
    add<impl::PressableAndClickableElement>();
}

} }
