#include <stdafx.h>
#include <gamebase/engine/ProjectionTransform.h>
#include "src/graphics/State.h"

namespace gamebase {

Transform2 projectionTransform()
{
    const State& curState = state();
    return ScalingTransform2(2.0f / curState.width, 2.0f / curState.height);
}

Size screenSize()
{
    const State& curState = state();
    return Size(curState.width, curState.height);
}

Vec2 convertMouseCoords(int x, int y)
{
    Vec2 mouseSourceCoords(static_cast<float>(x), static_cast<float>(y));
    Size scrnSize = screenSize();
    return Vec2(mouseSourceCoords.x - 0.5f * scrnSize.width, 0.5f * scrnSize.height - mouseSourceCoords.y);
}

}
