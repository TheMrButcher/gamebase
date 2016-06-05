#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/math/Vector2.h>

namespace gamebase { namespace impl {

GAMEBASE_API bool isPointInTriangle(const Vec2& point,
    const Vec2& p1, const Vec2& p2, const Vec2& p3);

} }
