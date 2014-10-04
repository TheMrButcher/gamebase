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
    return Size(
        static_cast<float>(curState.width),
        static_cast<float>(curState.height));
}

}
