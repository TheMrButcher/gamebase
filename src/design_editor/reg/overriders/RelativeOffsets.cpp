/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include "RelativeOffsets.h"
#include <reg/tools.h>

namespace gamebase { namespace editor {

void serializeFixedOffsetAsAligning(
    const impl::IObject* obj, impl::Serializer& s)
{
    if (auto offset = dynamic_cast<const impl::FixedOffset*>(obj)) {
        Vec2 v = offset->get();
        s   << "horAlign" << impl::HorAlign::None
            << "vertAlign" << impl::VertAlign::None
            << "horOffset" << pixels(v.x)
            << "vertOffset" << pixels(v.y);
    } else {
        THROW_EX() << "Serialzier expected FixedOffset as input";
    }
}

std::unique_ptr<impl::IObject> deserializeOffset(impl::Deserializer& deserializer)
{
    using namespace impl;
    DESERIALIZE(HorAlign::Enum, horAlign);
    DESERIALIZE(VertAlign::Enum, vertAlign);
    DESERIALIZE(RelativeValue, horOffset);
    DESERIALIZE(RelativeValue, vertOffset);
    std::unique_ptr<IObject> result;
    if (horAlign == HorAlign::None && vertAlign == VertAlign::None
        && horOffset.type() == RelType::Pixels && vertOffset.type() == RelType::Pixels) {
        result.reset(new FixedOffset(horOffset.value(), vertOffset.value()));
    } else {
        result.reset(new AligningOffset(horAlign, vertAlign, horOffset, vertOffset));
    }
    return std::move(result);
}

} }
