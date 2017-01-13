/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "ResultBuilderRegisterSnapshot.h"
#include <reg/overriders/RelativeOffsets.h>
#include <reg/overriders/RelativeBoxes.h>
#include <reg/overriders/GameObj.h>
#include <reg/overriders/SelectableElements.h>
#include <reg/overriders/LinearLayout.h>

namespace gamebase { namespace editor {

namespace {
void notSupported(const impl::IObject*, impl::Serializer&)
{
	THROW_EX() << "This function should not be called";
}
}

ResultBuilderRegisterSnapshot::ResultBuilderRegisterSnapshot()
    : RegisterSnapshot(RegisterSwitcher::ResultReg)
{
    add<impl::FixedOffset>();
    add<impl::AligningOffset>();

    add<impl::RelativeBox>();
    add<impl::OffsettedBox>();
    add<impl::SquareBox>();
    add<impl::FixedBox>();
    add<impl::PixelBox>();
    add<impl::ComplexBox>();

    add<impl::InactiveObjectConstruct>();
    add<impl::AnimatedObjectConstruct>();
    add<impl::ObjectConstruct>(&deserializeObjectConstruct);

    add<impl::PressableElement>();
    add<impl::ClickableElement>();
    add<impl::PressableAndClickableElement>();

    add<impl::LinearLayout>();
}

} }
