#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/math/Transform2.h>
#include <gamebase/utils/Size.h>

namespace gamebase {

GAMEBASE_API Transform2 projectionTransform();

GAMEBASE_API Size screenSize();

GAMEBASE_API Vec2 mouseCoordsToNormalized(int x, int y);

}
