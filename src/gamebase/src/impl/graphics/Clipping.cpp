/**
 * Copyright (c) 2017 Slavnejshev Filipp
 * This file is licensed under the terms of the MIT license.
 */

#include <stdafx.h>
#include "State.h"
#include <gamebase/impl/graphics/Clipping.h>
#include <gamebase/math/Math.h>

namespace gamebase { namespace impl {

namespace {
bool isClipperEnabled = false;
std::vector<BoundingBox> clipBoxes;
}

void pushClipBox(const Transform2& pos, const BoundingBox& box)
{
    if (!isClipperEnabled) {
        glEnable(GL_SCISSOR_TEST);
        isClipperEnabled = true;
    }

    const State& curState = state();
    auto fullTransform = pos * Transform2(
        ScalingMatrix2(0.5f * curState.width, 0.5f * curState.height),
        Vec2(0.5f * curState.width, 0.5f * curState.height));
    auto clipBox = box;
    clipBox.transform(fullTransform);
    if (!clipBoxes.empty())
        clipBox = intersect(clipBox, clipBoxes.back());

    int x1 = static_cast<int>(clipBox.bottomLeft.x);
    int y1 = static_cast<int>(clipBox.bottomLeft.y);
    int x2 = static_cast<int>(std::ceil(clipBox.topRight.x));
    int y2 = static_cast<int>(std::ceil(clipBox.topRight.y));
    glScissor(x1, y1, x2 - x1, y2 - y1);
    clipBoxes.push_back(clipBox);
}

void popClipBox()
{
    clipBoxes.pop_back();
    if (clipBoxes.empty()) {
        disableClipping();
    } else {
        auto clipBox = clipBoxes.back();
        glScissor(
            round(clipBox.bottomLeft.x), round(clipBox.bottomLeft.y),
            uround(clipBox.width()), uround(clipBox.height()));
    }
}

void resetClipper()
{
    clipBoxes.clear();
    disableClipping();
}

void disableClipping()
{
    if (isClipperEnabled) {
        glDisable(GL_SCISSOR_TEST);
        isClipperEnabled = false;
    }
}

} }
