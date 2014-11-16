#include <stdafx.h>
#include <gamebase/engine/ProjectionTransform.h>
#include "src/graphics/State.h"

namespace gamebase {

Transform2 projectionTransform()
{
    const State& curState = state();
    return ScalingTransform2(1.0f / curState.width, 1.0f / curState.height);
}

Size screenSize()
{
    const State& curState = state();
    return Size(curState.width, curState.height);
}

Vec2 mouseCoordsToNormalized(int x, int y)
{
    Vec2 mouseSourceCoords(static_cast<float>(x), static_cast<float>(y));
    Size scrnSize = screenSize();
    return projectionTransform()
        * Vec2(2.0f * mouseSourceCoords.x - scrnSize.width, scrnSize.height - 2.0f * mouseSourceCoords.y);
}

}
