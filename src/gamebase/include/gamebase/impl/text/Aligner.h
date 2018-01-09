/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/text/AlignedString.h>
#include <gamebase/impl/text/AlignProperties.h>

namespace gamebase { namespace impl {

GAMEBASE_API std::vector<AlignedString> alignText(
    std::string text,
    const AlignProperties& alignProps,
    const BoundingBox& box);

} }
