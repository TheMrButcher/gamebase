#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/text/AlignedString.h>
#include <gamebase/impl/text/AlignProperties.h>

namespace gamebase { namespace impl {

GAMEBASE_API std::vector<AlignedString> alignText(
    const std::string& text,
    const AlignProperties& alignProps,
    const BoundingBox& box);

} }
