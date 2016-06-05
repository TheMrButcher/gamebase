#pragma once

#include <gamebase/GameBaseAPI.h>
#include <gamebase/geom/BoundingBox.h>
#include <gamebase/math/Transform2.h>

namespace gamebase { namespace impl {

GAMEBASE_API void pushClipBox(const Transform2& pos, const BoundingBox& box);
GAMEBASE_API void popClipBox();
GAMEBASE_API void resetClipper();
GAMEBASE_API void disableClipping();

} }
