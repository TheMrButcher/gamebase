/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/impl/geom/BoundingBox.h>
#include <gamebase/math/Transform2.h>

namespace gamebase { namespace impl {

GAMEBASE_API void pushClipBox(const Transform2& pos, const BoundingBox& box);
GAMEBASE_API void popClipBox();
GAMEBASE_API void resetClipper();
GAMEBASE_API void disableClipping();

} }
