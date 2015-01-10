#include <stdafx.h>
#include "State.h"
#include <gamebase/graphics/Clipping.h>
#include <gamebase/math/Math.h>

namespace gamebase {

void setClipBox(const Transform2& pos, const BoundingBox& box)
{
    glEnable(GL_SCISSOR_TEST);

    const State& curState = state();
    auto fullTransform = pos * Transform2(
        ScalingMatrix2(0.5f * curState.width, 0.5f * curState.height),
        Vec2(0.5f * curState.width, 0.5f * curState.height));
    auto clipBox = box.transformed(fullTransform);
    glScissor(
        round(clipBox.bottomLeft.x), round(clipBox.bottomLeft.y),
        uround(clipBox.width()), uround(clipBox.height()));
}

void disableClipping()
{
    glDisable(GL_SCISSOR_TEST);
}

}
