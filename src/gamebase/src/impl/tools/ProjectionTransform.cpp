/**
 * Copyright (c) 2016 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include <gamebase/impl/tools/ProjectionTransform.h>
#include "src/impl/graphics/State.h"

namespace gamebase { namespace impl {

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

} }
