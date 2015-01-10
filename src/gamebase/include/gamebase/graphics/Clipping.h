#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/geom/BoundingBox.h>
#include <gamebase/math/Transform2.h>

namespace gamebase {

GAMEBASE_API void setClipBox(const Transform2& pos, const BoundingBox& box);

GAMEBASE_API void disableClipping();

}
