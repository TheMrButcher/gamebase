/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "ResultBuilderRegisterSnapshot.h"
#include <reg/overriders/RelativeOffsets.h>
#include <reg/overriders/RelativeBoxes.h>

namespace gamebase { namespace editor {

ResultBuilderRegisterSnapshot::ResultBuilderRegisterSnapshot()
    : RegisterSnapshot(RegisterSwitcher::DVBReg)
{
    add<impl::FixedOffset>("AligningOffset", &deserializeOffset, &serializeFixedOffsetAsAligning);
    add<impl::AligningOffset>(&deserializeOffset);
        
    add<impl::RelativeBox>("ComplexBox", &deserializeBox, &serializeRelativeBoxAsComplex);
    add<impl::OffsettedBox>("ComplexBox", &deserializeBox, &serializeOffsettedBoxAsComplex);
    add<impl::SquareBox>("ComplexBox", &deserializeBox, &serializeSquareBoxAsComplex);
    add<impl::FixedBox>("ComplexBox", &deserializeBox, &serializeFixedBoxAsComplex);
    add<impl::PixelBox>("ComplexBox", &deserializeBox, &serializePixelBoxAsComplex);
    add<impl::ComplexBox>(&deserializeBox);
}

} }
