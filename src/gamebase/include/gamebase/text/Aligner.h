#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/text/AlignedString.h>
#include <gamebase/text/AlignProperties.h>

namespace gamebase {

GAMEBASE_API std::vector<AlignedString> alignText(
    const std::string& text,
    const AlignProperties& alignProps,
    const BoundingBox& box);

}
