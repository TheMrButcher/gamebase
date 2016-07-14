/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/math/Vector2.h>

namespace gamebase { namespace impl {

GAMEBASE_API bool isPointInTriangle(const Vec2& point,
    const Vec2& p1, const Vec2& p2, const Vec2& p3);

} }
