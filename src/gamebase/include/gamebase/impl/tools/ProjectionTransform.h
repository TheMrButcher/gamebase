/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/math/Transform2.h>
#include <gamebase/tools/Size.h>

namespace gamebase { namespace impl {

GAMEBASE_API Transform2 projectionTransform();

GAMEBASE_API Size screenSize();

GAMEBASE_API Vec2 convertMouseCoords(int x, int y);

} }
